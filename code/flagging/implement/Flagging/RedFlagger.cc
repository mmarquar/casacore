//# RedFlagger.cc: this defines RedFlagger
//# Copyright (C) 2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/MeasurementSets/MSColumns.h>
#include <aips/MeasurementSets/MSSpWindowColumns.h>
#include <aips/Mathematics/Complex.h>
#include <aips/Measures/Stokes.h>
#include <aips/Utilities/Regex.h>
#include <aips/Tasking/AppInfo.h>
#include <trial/Flagging/RedFlagger.h>
#include <trial/Flagging/RFAMedianClip.h>
#include <trial/Flagging/RFASpectralRej.h>
#include <trial/Flagging/RFASelector.h>
#include <trial/Flagging/RFAUVBinner.h>
#include <trial/MeasurementEquations/VisibilityIterator.h>
#include <trial/MeasurementEquations/VisBuffer.h>
#include <trial/Tasking/PGPlotter.h>
#include <trial/Tasking/ProgressMeter.h>
#include <stdio.h>
#include <math.h>

 LogIO RedFlagger::os( LogOrigin("RedFlagger") );
 static char str[256];
 uInt debug_ifr=9999,debug_itime=9999;

// -----------------------------------------------------------------------
// Default Constructor
// -----------------------------------------------------------------------
RedFlagger::RedFlagger ()
{
  nant=0;
  setupAgentDefaults();
}

// -----------------------------------------------------------------------
// Constructor
// constructs and attaches to MS
// -----------------------------------------------------------------------
RedFlagger::RedFlagger ( const MeasurementSet &mset )
{
  nant=0;
  setupAgentDefaults();
  attach(mset);
}

// -----------------------------------------------------------------------
// queryOptions
// Returns record of available options and their default values
// -----------------------------------------------------------------------
const RecordInterface & RedFlagger::defaultOptions ()
{
  static Record rec;
// create record description on first entry
  if( !rec.nfields() )
  {
    Vector<Int> plotscr(2,3); 
    rec.define(RF_PLOTSCR,plotscr);
    rec.define(RF_PLOTDEV,plotscr);
    rec.define(RF_DEVFILE,"flagreport.ps/ps");
    rec.defineRecord(RF_GLOBAL,Record());
    
    rec.setComment(RF_PLOTSCR,"Format of screen plots: [NX,NY] or False to disable");
    rec.setComment(RF_PLOTDEV,"Format of hardcopy plots: [NX,NY], or False to disable");
    rec.setComment(RF_DEVFILE,"Filename for hardcopy (a PGPlot 'filename/device')");
    rec.setComment(RF_GLOBAL,"Record of global parameters applied to all agents");
  }
  return rec;
}

// -----------------------------------------------------------------------
// RedFlagger::attach
// attaches to MS
// -----------------------------------------------------------------------
void RedFlagger::attach( const MeasurementSet &mset )
{
  ms = mset;
// extract various interesting info from the MS
// obtain number of distinct time slots
  ROMSColumns msc(ms);
  Vector<Double> time( msc.time().getColumn() );
  uInt nrows = time.nelements();
  Bool dum;
  Sort sort( time.getStorage(dum),sizeof(Double) );
  sort.sortKey((uInt)0,TpDouble);
  Vector<uInt> index,uniq;
  sort.sort(index,time.nelements());
  ntime = sort.unique(uniq,index);
// obtain number of antennas and interferometers
  const MSAntenna msant( ms.antenna() );
  nant = msant.nrow();
  nifr = nant*(nant+1)/2; // cheap & dirty
  ROScalarColumn<String> names(msant,"NAME");
  antnames = names.getColumn();
  antnames.apply(stringUpper);
//  cerr<<"Antenna names: "<<antnames<<endl;
// map ifrs to antennas
  ifr2ant1.resize(nifr);
  ifr2ant1.set(-1);
  ifr2ant2.resize(nifr);
  ifr2ant2.set(-1);
  for( uInt i1=0; i1<nant; i1++ )
    for( uInt i2=0; i2<=i1; i2++ )
    {
      uInt ifr = ifrNumber(i1,i2);
      ifr2ant1(ifr) = i1;
      ifr2ant2(ifr) = i2;
    }
  sprintf(str,"attached MS %s: %d rows, %d times, %d IFRs\n",ms.tableName().chars(),nrows,ntime,nifr);
  os<<str<<LogIO::POST;
}    

// computes IFR index, given two antennas
uInt RedFlagger::ifrNumber ( Int ant1,Int ant2 ) const
{
  if( ant1<ant2 )
    return ifrNumber(ant2,ant1);
  return ant1*(ant1+1)/2 + ant2;
}

// computes vector of IFR indeces, given two antennas
Vector<Int> RedFlagger::ifrNumbers ( Vector<Int> ant1,Vector<Int> ant2 ) const
{
  Vector<Int> a1( max(ant1,ant2) ),
             a2( min(ant1,ant2) );
  return a1*(a1+1)/2 + a2;
}

void RedFlagger::ifrToAnt ( uInt &ant1,uInt &ant2,uInt ifr ) const
{
  ant1 = ifr2ant1(ifr);
  ant2 = ifr2ant2(ifr);
}

// -----------------------------------------------------------------------
// RedFlagger::setupAgentDefaults
// Sets up record of available agents and their default parameters
// -----------------------------------------------------------------------
const RecordInterface & RedFlagger::setupAgentDefaults ()
{
  agent_defaults = Record();
  agent_defaults.defineRecord("timemed",RFATimeMedian::getDefaults());
  agent_defaults.defineRecord("freqmed",RFAFreqMedian::getDefaults());
  agent_defaults.defineRecord("sprej",RFASpectralRej::getDefaults());
  agent_defaults.defineRecord("select",RFASelector::getDefaults());
  agent_defaults.defineRecord("uvbin",RFAUVBinner::getDefaults());
  return agent_defaults;
}

// -----------------------------------------------------------------------
// RedFlagger::createAgent
// Creates flagging agent based on name
// -----------------------------------------------------------------------
RFABase * RedFlagger::createAgent ( const String &id,RFChunkStats &chunk,const RecordInterface &parms )
{
  if( id == "timemed" )
    return new RFATimeMedian(chunk,parms);
  else if( id == "freqmed" )
    return new RFAFreqMedian(chunk,parms);
  else if( id == "sprej" )
    return new RFASpectralRej(chunk,parms);
  else if( id == "select" )
    return new RFASelector(chunk,parms);
  else if( id == "uvbin" )
    return new RFAUVBinner(chunk,parms);
  else
    return NULL;
}

// -----------------------------------------------------------------------
// RedFlagger::run
// Performs the actual flagging
// -----------------------------------------------------------------------
void RedFlagger::run ( const RecordInterface &agents,const RecordInterface &opt,uInt ind_base ) 
{
  if( !nant )
    os<<"No Measurement Set has been attached\n"<<LogIO::EXCEPTION;
  RFABase::setIndexingBase(ind_base);  
// set debug level
  Int debug_level=0;
  if( opt.isDefined("debug") )
    debug_level = opt.asInt("debug");
  
// setup plotting devices
  PGPlotter pgp_screen,pgp_report;
  setupPlotters(pgp_screen,pgp_report,opt);
  
// create iterator, visbuffer & chunk manager
  Block<Int> sortCol(1);
  sortCol[0] = MeasurementSet::TIME;
  VisibilityIterator vi(ms,sortCol,1000000000);
  VisBuffer vb(vi);
  RFChunkStats chunk(vi,vb,*this,&pgp_screen,&pgp_report);

// setup global options for flagging agents
  Record globopt(Record::Variable);
  if( opt.isDefined(RF_GLOBAL) )
    globopt = opt.asRecord(RF_GLOBAL);

// clean up any dead agents from previous run  
  for( uInt i=0; i<acc.nelements(); i++ )
    if( acc[i] )
      delete acc[i];

// generate new array of agents by iterating through agents record
  Record agcounts; // record of agent instance counts
  acc.resize(agents.nfields());
  acc.set(NULL);
  uInt nacc = 0;
  for( uInt i=0; i<agents.nfields(); i++ ) 
  {
    if(  agents.dataType(i) != TpRecord )
      os<<"Unrecognized field '"<<agents.name(i)<<"' in agents\n"<<LogIO::EXCEPTION;
    const RecordInterface & agent_rec( agents.asRecord(i) );
    // normally, the field name itself is the agent ID
    String agent_id( downcase(agents.name(i)) );
    // but if an id field is set in the sub-record, use that instead
    if( agent_rec.isDefined("id") && agent_rec.dataType("id") == TpString )
      agent_id = agents.asRecord(i).asString("id");
    // check that this is agent really exists
    if( !agent_defaults.isDefined(agent_id) )
      os<<"Unknown flagging agent '"<<agents.name(i)<<"'\n"<<LogIO::EXCEPTION;
    // create parameter record by taking agent defaults, and merging in global
    // and specified options
    const RecordInterface & defparms(agent_defaults.asRecord(agent_id));
    Record parms(defparms);
    parms.merge(globopt,Record::OverwriteDuplicates); 
    parms.merge(agent_rec,Record::OverwriteDuplicates);
    // see if this is a different instance of an already activated agent
    if( agcounts.isDefined(agent_id) )
    {
      // increment the instance counter
      Int count = agcounts.asInt(agent_id)+1;
      agcounts.define(agent_id,count);
      // modify the agent name to include an instance count
      char s[128];
      sprintf(s,"%s#%d",defparms.asString(RF_NAME).chars(),count);
      parms.define(RF_NAME,s);
    }
    else
      agcounts.define(agent_id,1);
    // create agent based on name
    RFABase *agent = createAgent(agent_id,chunk,parms);
    if( !agent )
      os<<"Unrecognized agent name '"<<agents.name(i)<<"'\n"<<LogIO::EXCEPTION;
    agent->init();
    String inp,st;
//    agent->logSink()<<agent->getDesc()<<endl<<LogIO::POST;
    acc[nacc++] = agent;
  }
  acc.resize(nacc);
  
// begin iterating over chunks
  uInt nchunk=0;
  for( vi.originChunks(); vi.moreChunks(); vi.nextChunk(),nchunk++ ) 
  {
    chunk.newChunk();
// How much memory do we have?
    Int availmem = opt.isDefined("maxmem") ? 
        opt.asInt("maxmem") : AppInfo::memoryInMB();
    if( debug_level>0 )
      fprintf(stderr,"%d MB memory available\n",availmem);
// see if a flag cube is being used, and tell it to use/not use memory
    if( RFFlagCube::numInstances() )
    {
      Int flagmem = RFFlagCube::estimateMemoryUse(chunk);
      // memory tight? use a disk-based flag cube
      if( flagmem>.75*availmem )
      {
        if( debug_level>0 )
          fprintf(stderr,"%d MB flag cube: using disk\n",flagmem);
        RFFlagCube::setMaxMem(0);
        availmem -= 2; // reserve 2 MB for the iterator
      }
      else // else use an in-memory cube
      {
        if( debug_level>0 )
          fprintf(stderr,"%d MB flag cube: using memory\n",flagmem);
        RFFlagCube::setMaxMem(availmem);
        availmem -= flagmem;
      }
    }
// call newChunk() for all accumulators; determine which ones are active
    Vector<Int> iter_mode(acc.nelements(),RFA::DATA);
    Vector<Bool> active(acc.nelements());
    for( uInt i = 0; i<acc.nelements(); i++ ) 
    {
      Int maxmem;
      maxmem = availmem;
      if( ! (active(i) = acc[i]->newChunk(maxmem))  ) // refused this chunk?
        iter_mode(i) = RFA::STOP;  // skip over it
      else
      { // active, so reserve its memory 
        if( debug_level>0 )
          fprintf(stderr,"%s reserving %d MB of memory, %d left in pool\n",
              acc[i]->name().chars(),availmem-maxmem,maxmem);
        availmem = maxmem>0 ? maxmem : 0;
      }
    }
    if( !sum(active) )
    {
      os<<LogIO::WARN<<"Unable to process this chunk with any agent.\n"<<LogIO::NORMAL;
      continue;
    }
// initially active agents
    Vector<Bool> active_init = active;
// start executing passes    
    char subtitle[128];
    sprintf(subtitle,"Flagging %s chunk %d: ",ms.tableName().chars(),nchunk+1);
    String title(subtitle);
    for( uInt npass=0; anyNE(iter_mode,(Int)RFA::STOP); npass++ ) // repeat passes while someone is active
    {
      uInt tcount=0;
      chunk.newPass(npass);
  // count up who wants a data pass and who wants a dry pass    
      Int ndata = sum(iter_mode==(Int)RFA::DATA);
      Int ndry  = sum(iter_mode==(Int)RFA::DRY);
      Int nactive = ndata+ndry;
      if( !nactive ) // no-one? break out then
        break;
  // Decide when to schedule a full data iteration, and when do dry runs only.
  // There's probably room for optimizations here, but let's keep it simple 
  // for now: since data iterations are more expensive, hold them off as long
  // as someone is requesting a dry run.
      Bool data_pass = !ndry;
  // Doing a full data iteration    
      if( data_pass )
      {
        sprintf(subtitle,"pass %d (data)",npass+1);
        ProgressMeter progmeter(1,chunk.num(TIME),title+subtitle,"","","");
        // start pass for all active agents
        for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
          if( active(ival) )
            if( iter_mode(ival) == RFA::DATA )
              acc[ival]->startData();
            else if( iter_mode(ival) == RFA::DRY )
              acc[ival]->startDry();
        // iterate over visbuffers
        for( vi.origin(); vi.more() && nactive; vi++,tcount++ )
        {
          progmeter.update(tcount);
          chunk.newTime();
          // now, call individual VisBuffer iterators
          for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
            if( active(ival) )
            {
              // call iterTime/iterDry as appropriate
              RFA::IterMode res = RFA::STOP;
              if( iter_mode(ival) == RFA::DATA )
                res = acc[ival]->iterTime(tcount);
              else if( iter_mode(ival) == RFA::DRY ) 
                res = acc[ival]->iterDry(tcount);
              // change requested? Deactivate agent
              if( ! ( res == RFA::CONT || res == iter_mode(ival) ) )
              {
                active(ival) = False;
                nactive--;
                iter_mode(ival)==RFA::DATA ? ndata-- : ndry--;
                iter_mode(ival) = res;
                if( nactive <= 0 )
                  break;
              }
            }
          // also iterate over rows for data passes
          for( Int ir=0; ir<vb.nRow() && ndata; ir++ )
            for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
              if( iter_mode(ival) == RFA::DATA )
              {
                RFA::IterMode res = acc[ival]->iterRow(ir);
                if( ! ( res == RFA::CONT || res == RFA::DATA ) )
                {
                  ndata--; nactive--;
                  iter_mode(ival) = res;
                  active(ival) = False;
                  if( ndata <= 0 )
                    break;
                }
              }
        }
        // end pass for all agents
        for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
        {
          if( active(ival) )
            if( iter_mode(ival) == RFA::DATA )
              iter_mode(ival) = acc[ival]->endData();
            else if( iter_mode(ival) == RFA::DRY )
              iter_mode(ival) = acc[ival]->endDry();
        }
      }
      else  // dry pass only
      {
        sprintf(subtitle,"pass %d (dry)",npass+1);
        ProgressMeter progmeter(1,chunk.num(TIME),title+subtitle,"","","");
        // start pass for all active agents
        for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
          if( iter_mode(ival) == RFA::DRY )
            acc[ival]->startDry();
//        uInt ntime = vi.nSubIntervals();
        for( uInt itime=0; itime<ntime && ndry; itime++ )
        {
          progmeter.update(itime);
          // now, call individual VisBuffer iterators
          for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
            if( iter_mode(ival) == RFA::DRY )
            {
              // call iterTime/iterDry as appropriate
              RFA::IterMode res = acc[ival]->iterDry(itime);
              // change requested? Deactivate agent
              if( ! ( res == RFA::CONT || res == RFA::DRY ) )
              {
                iter_mode(ival) = res;
                active(ival) = False;
                if( --ndry <= 0 )
                  break;
              }
            }
        }
        // end pass for all agents
        for( uInt ival = 0; ival<acc.nelements(); ival++ ) 
          if( iter_mode(ival) == RFA::DRY )
            iter_mode(ival) = acc[ival]->endDry();
      } // end of dry pass
    } // end loop over passes
// now, do a single flag-transfer pass to transfer flags into MS
    if( anyNE(active_init,False) )
    {
      ProgressMeter progmeter(1,chunk.num(TIME),title+"storing flags","","","");
      for( uInt i = 0; i<acc.nelements(); i++ ) 
        if( active_init(i) )
          acc[i]->startFlag();
      uInt itime=0;
      for( vi.origin(); vi.more(); vi++,itime++ )
      {
        progmeter.update(itime);
        chunk.newTime();
        for( uInt i = 0; i<acc.nelements(); i++ ) 
          if( active_init(i) )
            acc[i]->iterFlag(itime);
      }
      for( uInt i = 0; i<acc.nelements(); i++ ) 
        if( active_init(i) )
          acc[i]->endFlag();
    }
// generate reports
    if( pgp_report.isAttached() )
    {
      pgp_report.subp(3,3);
      plotSummaryReport(pgp_report,chunk);
      plotAgentReports(pgp_report);
    }
    if( pgp_screen.isAttached() )
    {
      plotAgentReports(pgp_screen);
    }
    
// call endChunk on all agents
    for( uInt i = 0; i<acc.nelements(); i++ ) 
      acc[i]->endChunk();
    
  } // end loop over chunks
  
// delete all chunks
  for( uInt i=0; i<acc.nelements(); i++ )
  {
    if( acc[i] )
    {
      delete acc[i];
      acc[i] = NULL;
    }
  }
  acc.resize(0);
      
  os<<"Flagging complete\n"<<LogIO::POST;
}

// -----------------------------------------------------------------------
// RedFlagger::setupPlotters
// Sets up screen and hardcopy plotters according to options
// -----------------------------------------------------------------------
void RedFlagger::setupPlotters ( PGPlotter &pgp_screen,PGPlotter &pgp_report,const RecordInterface &opt )
{
  if( fieldType(opt,RF_PLOTSCR,TpBool) && !opt.asBool(RF_PLOTSCR) )
  { 
    // skip the on-screen plot report
  }
  else  // else generate report
  {
    pgp_screen = PGPlotter("/xw",80);
    // setup colormap for PS
    uInt c1=16,nc=64;
    Float scale=1.0/(nc-1);
    pgp_screen.scir(c1,c1+nc-1);
    for( uInt c=0; c<nc; c++ )
      pgp_screen.scr(c1+c,c*scale,c*scale,c*scale);
    if( fieldType(opt,RF_PLOTSCR,TpArrayInt) )
    {
      Vector<Int> subp( opt.asArrayInt(RF_PLOTSCR) );
      pgp_screen.subp(subp(0),subp(1)); 
    }
    else
      pgp_screen.subp(3,3);
  }
// Device for hardcopy report 
//   plotdev=F for no plot
//   plotdev=T for plot (*default*)
//   plotdev=[nx,ny] for NX x NY sub-panels
  if( fieldType(opt,RF_PLOTDEV,TpBool) && !opt.asBool(RF_PLOTDEV) )
  {
    // skip the hardcopy report
  }
  else 
  {
    String filename( defaultOptions().asString(RF_DEVFILE) );
    if( fieldType(opt,RF_DEVFILE,TpString) )
      filename = opt.asString(RF_DEVFILE);
    if( filename.length() )
    {
      // make sure default device is "/ps"
      if( !filename.contains(Regex("/[a-zA-Z0-9]+$")) ) 
        filename += "/ps";
      pgp_report = PGPlotter(filename,80);
      // setup colormap for PS
      uInt c1=16,nc=64;
      Float scale=1.0/(nc-1);
      pgp_report.scir(c1,c1+nc-1);
      for( uInt c=0; c<nc; c++ )
        pgp_report.scr(c1+c,c*scale,c*scale,c*scale);
      // setup pane layout
      if( fieldType(opt,RF_PLOTDEV,TpArrayInt) )
      {
        Vector<Int> subp( opt.asArrayInt(RF_PLOTDEV) );
        pgp_report.subp(subp(0),subp(1)); 
      }
      else
        pgp_report.subp(3,3);
    }
  }
}

// -----------------------------------------------------------------------
// PlotSummaryReport
// Generates a summary flagging report for current chunk
// -----------------------------------------------------------------------
void RedFlagger::plotSummaryReport ( PGPlotterInterface &pgp,RFChunkStats &chunk )
{
// generate a short text report in the first pane
  pgp.env(0,1,0,1,0,-2);
  char s[128];
  sprintf(s,"Flagging report for MS '%s' chunk %d",ms.tableName().chars(),chunk.nchunk());
  pgp.lab("","",s);

  Float y0=1,dy=(pgp.qcs(4))(1)*1.5; // dy is text baseline height
  Vector<Float> vec01(2);
  vec01(0)=0; vec01(1)=1;

  // print overall flagging stats
  uInt n=0,n0;
  for( uInt i=0; i<chunk.num(IFR); i++ )
    if( chunk.nrowPerIfr(i) )
      n++;
  sprintf(s,"%d rows by %d channels by %d correlations. %d time slots, %d active IFRs",
      chunk.num(ROW),chunk.num(CHAN),chunk.num(CORR),chunk.num(TIME),n);
  pgp.text(0,y0-=dy,s);
  n  = sum(chunk.nrfIfr());
  n0 = chunk.num(ROW);
  sprintf(s,"%d (%0.2f%%) rows have been flagged.",n,n*100.0/n0);
  pgp.text(0,y0-=dy,s);
  n  = sum(chunk.nfIfrTime());
  n0 = chunk.num(ROW)*chunk.num(CHAN)*chunk.num(CORR);
  sprintf(s,"%d of %d (%0.2f%%) pixels have been flagged.",n,n0,n*100.0/n0);
  pgp.text(0,y0-=dy,s);
  pgp.line(vec01,Vector<Float>(2,y0-dy/4));

  // print per-agent flagging summary
  for( uInt i=0; i<acc.nelements(); i++ )
  {
    pgp.text(0,y0-=dy,acc[i]->name()+": "+acc[i]->getDesc());
    if( acc[i]->isActive() )
      pgp.text(0,y0-=dy,String("     ")+acc[i]->getStats());
    else
      pgp.text(0,y0-=dy,String("     can't process this chunk"));
  }
  pgp.line(vec01,Vector<Float>(2,y0-dy/4));
  pgp.iden();
}

// -----------------------------------------------------------------------
// plotAgentReport
// Generates per-agent reports for current chunk of data
// Meant to be called before doing endChunk() on all the flagging 
// agents.
// -----------------------------------------------------------------------
void RedFlagger::plotAgentReports( PGPlotterInterface &pgp )
{
  if( !pgp.isAttached() )
    return;
// call each agent to produce summary plots
  for( uInt i=0; i<acc.nelements(); i++ )
    acc[i]->plotFlaggingReport(pgp);
}

