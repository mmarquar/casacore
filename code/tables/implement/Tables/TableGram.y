/*
    TableGram.y: Parser for table commands
    Copyright (C) 1994,1995,1997,1998,1999,2001,2002,2003
    Associated Universities, Inc. Washington DC, USA.

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; if not, write to the Free Software Foundation,
    Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.

    Correspondence concerning AIPS++ should be addressed as follows:
           Internet email: aips2-request@nrao.edu.
           Postal address: AIPS++ Project Office
                           National Radio Astronomy Observatory
                           520 Edgemont Road
                           Charlottesville, VA 22903-2475 USA

    $Id$
*/

%pure_parser                /* make parser re-entrant */

%union {
TableExprNode* node;
TableParseVal* val;
Block<TableExprNode>* exprb;
TableExprNodeSetElem* elem;
TableExprNodeSet* settp;
PtrBlock<TableParseSort*>* sortb;
TableParseSort* sort;
TableParseSelect* select;
TableParseUpdate* update;
PtrBlock<TableParseUpdate*>* updateb;
}

%token SELECT
%token UPDATE
%token UPDSET
%token INSERT
%token VALUES
%token DELETE
%token FROM
%token WHERE
%token ORDERBY
%token NODUPL
%token GIVING
%token INTO
%token SORTASC
%token SORTDESC
%token LIMIT
%token OFFSET
%token ALL                  /* ALL (in SELECT ALL) or name of function */
%token <val> NAME           /* name of function or shorthand for table */
%token <val> FLDNAME        /* name of field or shorthand for table */
%token <val> TABNAME        /* table name */
%token <val> LITERAL
%token <val> STRINGLITERAL
%token AS
%token IN
%token BETWEEN
%token LIKE
%token LPAREN
%token RPAREN
%token COMMA
%token LBRACKET
%token RBRACKET
%token LBRACE
%token RBRACE
%token COLON
%token OPENOPEN
%token OPENCLOSED
%token CLOSEDOPEN
%token CLOSEDCLOSED
%token OPENEMPTY
%token EMPTYOPEN
%token CLOSEDEMPTY
%token EMPTYCLOSED
%type <val> tfname
%type <val> tabname
%type <node> whexpr
%type <node> orexpr
%type <node> andexpr
%type <node> relexpr
%type <node> arithexpr
%type <node> inxexpr
%type <node> simexpr
%type <node> set
%type <node> singlerange
%type <settp> subscripts
%type <settp> elemlist
%type <settp> elems
%type <elem> elem
%type <elem> subsrange
%type <elem> colonrange
%type <elem> range
%type <sort>  sortexpr
%type <sortb> sortlist
%type <update> updexpr
%type <update> insexpr
%type <updateb> updlist
%type <updateb> inslist


%left OR
%left AND
%nonassoc EQ EQASS GT GE LT LE NE
%left PLUS MINUS
%left TIMES DIVIDE MODULO
%nonassoc UNARY
%nonassoc NOT
%right POWER


%{
int TableGramlex (YYSTYPE*);
%}

%%
command:   selcomm
         | updcomm
         | inscomm
         | delcomm
         ;

selcomm:   select selrow
         ;

select:    SELECT {
               TableParseSelect::newSelect(1);
	   }
         ;

selrow:    selcol selwh order limitoff given
         | selcol into selwh order limitoff
         ;

selcol:    columns
         | TIMES           /* SELECT * FROM ... */
         | ALL columns
         | NODUPL columns {
	       TableParseSelect::currentSelect()->setDistinctCol();
           }
         ;

selwh:     FROM tables whexpr {
	       TableParseSelect::currentSelect()->handleSelect ($3);
	       delete $3;
           }
         ;

updcomm:   update updrow
         ;

update:    UPDATE {
               TableParseSelect::newSelect(2);
           }
         ;

updrow:    tables updpart updfrom whexpr order limitoff {
	       TableParseSelect::currentSelect()->handleSelect ($4);
	       delete $4;
           }
         ;

updpart:   UPDSET updlist {
               TableParseSelect::currentSelect()->handleUpdate ($2);
           }
         ;

updlist:   updlist COMMA updexpr {
               $$ = $1;
               $$->resize($$->nelements() + 1);
	       (*$$)[$$->nelements() - 1] = $3;
           }
         | updexpr {
	       $$ = new PtrBlock<TableParseUpdate*>(1);
	       (*$$)[0] = $1;
           }
         ;

updexpr:   NAME EQASS orexpr {
               $$ = new TableParseUpdate ($1->str, *$3);
	       delete $1;
	       delete $3;
           }
         | NAME LBRACKET subscripts RBRACKET EQASS orexpr {
               $$ = new TableParseUpdate ($1->str, *$3, *$6);
	       delete $1;
	       delete $3;
	       delete $6;
           }
         ;


updfrom:        /* no FROM */
         | FROM tables
         ;

inscomm:   insert insrow
         ;

insert:    INSERT {
               TableParseSelect::newSelect(3);
           }
         ;

insrow:    INTO tables insclist inspart
         ;

insclist:           /* no column-list */   
         | LBRACKET columns RBRACKET
         | LPAREN columns RPAREN
         ;

inspart:   VALUES LBRACKET inslist RBRACKET {
               TableParseSelect::currentSelect()->handleInsert ($3);
           }
         | VALUES LPAREN inslist RPAREN {
               TableParseSelect::currentSelect()->handleInsert ($3);
           }
         | selcomm {
	       TableParseSelect* p = TableParseSelect::popSelect();
               TableParseSelect::currentSelect()->handleInsert (p);
	   }
         ;

inslist:   inslist COMMA insexpr {
               $$ = $1;
               $$->resize($$->nelements() + 1);
	       (*$$)[$$->nelements() - 1] = $3;
           }
         | insexpr {
	       $$ = new PtrBlock<TableParseUpdate*>(1);
	       (*$$)[0] = $1;
           }
         ;

insexpr:   orexpr {
               $$ = new TableParseUpdate ("", *$1);
	       delete $1;
           }
         ;

delcomm:   delete selwh order limitoff
         ;

delete:    DELETE {
               TableParseSelect::newSelect(4);
           }
         ;

order:               /* no sort */
         | ORDERBY sortlist {
	       TableParseSelect::currentSelect()->handleSort ($2, False,
							     Sort::Ascending);
	   }
         | ORDERBY SORTASC sortlist {
	       TableParseSelect::currentSelect()->handleSort ($3, False,
							     Sort::Ascending);
	   }
         | ORDERBY SORTDESC sortlist {
	       TableParseSelect::currentSelect()->handleSort ($3, False,
							     Sort::Descending);
	   }
         | ORDERBY NODUPL sortlist {
	       TableParseSelect::currentSelect()->handleSort ($3, True,
							     Sort::Ascending);
	   }
         | ORDERBY NODUPL SORTASC sortlist {
	       TableParseSelect::currentSelect()->handleSort ($4, True,
							     Sort::Ascending);
	   }
         | ORDERBY NODUPL SORTDESC sortlist {
	       TableParseSelect::currentSelect()->handleSort ($4, True,
							     Sort::Descending);
	   }
         | ORDERBY SORTASC NODUPL sortlist {
	       TableParseSelect::currentSelect()->handleSort ($4, True,
							     Sort::Ascending);
	   }
         | ORDERBY SORTDESC NODUPL sortlist {
	       TableParseSelect::currentSelect()->handleSort ($4, True,
							     Sort::Descending);
	   }
         ;

limitoff:           /* no limit,offset */
         | LIMIT orexpr {
	       TableParseSelect::currentSelect()->handleLimit (*$2);
	       delete $2;
	   }
         | OFFSET orexpr {
	       TableParseSelect::currentSelect()->handleOffset (*$2);
	       delete $2;
	   }
         | LIMIT orexpr OFFSET orexpr {
	       TableParseSelect::currentSelect()->handleLimit (*$2);
	       TableParseSelect::currentSelect()->handleOffset (*$4);
	       delete $2;
	       delete $4;
	   }
         | OFFSET orexpr LIMIT orexpr {
	       TableParseSelect::currentSelect()->handleOffset (*$2);
	       TableParseSelect::currentSelect()->handleLimit (*$4);
	       delete $2;
	       delete $4;
	   }
         ;

given:               /* no result */
         | GIVING tabname {
               TableParseSelect::currentSelect()->handleGiving ($2->str);
	       delete $2;
	   }
         | GIVING LBRACKET elems RBRACKET {
               TableParseSelect::currentSelect()->handleGiving (*$3);
	       delete $3;
	   }
         ;

into:      INTO tabname {
               TableParseSelect::currentSelect()->handleGiving ($2->str);
	       delete $2;
	   }
         ;

columns:             /* no column names given (thus take all) */
         | NAME {
	       TableParseSelect::currentSelect()->handleSelectColumn ($1->str);
	       delete $1;
	   }
         | columns COMMA NAME {
	       TableParseSelect::currentSelect()->handleSelectColumn ($3->str);
	       delete $3;
	   }
         ;

tables:    NAME {                            /* table is shorthand */
	       TableParseSelect::currentSelect()->addTable ($1, $1->str);
	       delete $1;
	   }
         | tfname {                          /* no shorthand */
	       TableParseSelect::currentSelect()->addTable ($1, "");
	       delete $1;
	   }
	 | tabname NAME {
	       TableParseSelect::currentSelect()->addTable ($1, $2->str);
	       delete $1;
	       delete $2;
	   }
	 | tabname AS NAME {
	       TableParseSelect::currentSelect()->addTable ($1, $3->str);
	       delete $1;
	       delete $3;
	   }
	 | NAME IN tabname {
	       TableParseSelect::currentSelect()->addTable ($3, $1->str);
	       delete $1;
	       delete $3;
	   }
         | tables COMMA NAME {
	       TableParseSelect::currentSelect()->addTable ($3, $3->str);
	       delete $3;
	   }
         | tables COMMA tfname {
	       TableParseSelect::currentSelect()->addTable ($3, "");
	       delete $3;
	   }
	 | tables COMMA tabname NAME {
	       TableParseSelect::currentSelect()->addTable ($3, $4->str);
	       delete $3;
	       delete $4;
	   }
	 | tables COMMA tabname AS NAME {
	       TableParseSelect::currentSelect()->addTable ($3, $5->str);
	       delete $3;
	       delete $5;
	   }
	 | tables COMMA NAME IN tabname {
	       TableParseSelect::currentSelect()->addTable ($5, $3->str);
	       delete $3;
	       delete $5;
	   }
         ;

tfname:    TABNAME
               { $$ = $1; }
         | FLDNAME
               { $$ = $1; }
         | STRINGLITERAL
               { $$ = $1; }
         | LBRACKET selcomm RBRACKET {
	       TableParseSelect* p = TableParseSelect::popSelect();
	       $$ = p->doFromQuery();
	       theFromQueryDone = True;
	       delete p;
           }
         | LPAREN selcomm RPAREN {
	       TableParseSelect* p = TableParseSelect::popSelect();
	       $$ = p->doFromQuery();
	       theFromQueryDone = True;
	       delete p;
           }
         ;

tabname:   NAME
               { $$ = $1; }
         | tfname
               { $$ = $1; }
         ;   

whexpr:
	       { $$ = 0; }                      /* no selection */
	 | WHERE orexpr
	       { $$ = $2; }
	 ;

orexpr:    andexpr
         | orexpr OR andexpr {
	       $$ = new TableExprNode (*$1 || *$3);
	       delete $1;
	       delete $3;
	   }
         ;

andexpr:   relexpr
         | andexpr AND relexpr {
	       $$ = new TableExprNode (*$1 && *$3);
	       delete $1;
	       delete $3;
	   }
         ;

relexpr:   arithexpr
         | arithexpr EQ arithexpr {
	       $$ = new TableExprNode (*$1 == *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr EQASS arithexpr {
	       $$ = new TableExprNode (*$1 == *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr GT arithexpr {
	       $$ = new TableExprNode (*$1 > *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr GE arithexpr {
	       $$ = new TableExprNode (*$1 >= *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr LT arithexpr {
	       $$ = new TableExprNode (*$1 < *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr LE arithexpr {
	       $$ = new TableExprNode (*$1 <= *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr NE arithexpr {
	       $$ = new TableExprNode (*$1 != *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr LIKE arithexpr {
	       $$ = new TableExprNode (*$1 == sqlpattern(*$3));
	       delete $1;
	       delete $3;
	   }
         | arithexpr NOT LIKE arithexpr {
	       TableExprNode node (*$1 == sqlpattern(*$4));
	       $$ = new TableExprNode (!node);
	       delete $1;
	       delete $4;
	   }
         | arithexpr IN arithexpr {
               $$ = new TableExprNode ($1->in (*$3));
               delete $1;
               delete $3;
           }
         | arithexpr NOT IN arithexpr {
               TableExprNode node ($1->in (*$4));
               $$ = new TableExprNode (!node);
               delete $1;
               delete $4;
           }
         | arithexpr IN singlerange {
               $$ = new TableExprNode ($1->in (*$3));
               delete $1;
               delete $3;
           }
         | arithexpr NOT IN singlerange {
               TableExprNode node ($1->in (*$4));
               $$ = new TableExprNode (!node);
               delete $1;
               delete $4;
           }
         | arithexpr BETWEEN arithexpr AND arithexpr {
 	       TableExprNodeSet set;
	       set.add (TableExprNodeSetElem(True, *$3, *$5, True));
               $$ = new TableExprNode ($1->in (set));
               delete $1;
               delete $3;
	       delete $5;
           }
         | arithexpr NOT BETWEEN arithexpr AND arithexpr {
 	       TableExprNodeSet set;
	       set.add (TableExprNodeSetElem(True, *$4, *$6, True));
               TableExprNode node ($1->in (set));
               $$ = new TableExprNode (!node);
               delete $1;
               delete $4;
	       delete $6;
           }
         ;

arithexpr: inxexpr
         | arithexpr PLUS  arithexpr {
	       $$ = new TableExprNode (*$1 + *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr MINUS arithexpr {
	       $$ = new TableExprNode (*$1 - *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr TIMES  arithexpr {
	       $$ = new TableExprNode (*$1 * *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr DIVIDE arithexpr {
	       $$ = new TableExprNode (*$1 / *$3);
	       delete $1;
	       delete $3;
	   }
         | arithexpr MODULO arithexpr {
	       $$ = new TableExprNode (*$1 % *$3);
	       delete $1;
	       delete $3;
	   }
         | MINUS arithexpr %prec UNARY {
	       $$ = new TableExprNode (-*$2);
	       delete $2;
	   }
         | PLUS  arithexpr %prec UNARY
               { $$ = $2; }
         | NOT   arithexpr {
	       $$ = new TableExprNode (!*$2);
	       delete $2;
	   }
         | arithexpr POWER arithexpr {
	       $$ = new TableExprNode (pow (*$1, *$3));
	       delete $1;
	       delete $3;
	   }
         ;

inxexpr:   simexpr
         | simexpr LBRACKET subscripts RBRACKET {
	       $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleSlice (*$1, *$3));
	       delete $1;
	       delete $3;
	   }
         ;

simexpr:   LPAREN orexpr RPAREN
               { $$ = $2; }
         | NAME LPAREN elemlist RPAREN {
               $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleFunc ($1->str, *$3));
	       delete $1;
	       delete $3;
	   }
         | ALL LPAREN elemlist RPAREN {
               $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleFunc ("ALL", *$3));
	       delete $3;
	   }
         | NAME {
	       $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleKeyCol ($1->str));
	       delete $1;
	   }
         | FLDNAME {
	       $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleKeyCol ($1->str));
	       delete $1;
	   }
         | LITERAL {
	       $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleLiteral ($1));
	       delete $1;
	   }
         | STRINGLITERAL {
	       $$ = new TableExprNode (TableParseSelect::currentSelect()->
                                                 handleLiteral ($1));
	       delete $1;
	   }
         | set {
	       $$ = $1;
	   }
         ;

set:       LBRACKET elems RBRACKET {
               $$ = new TableExprNode ($2->setOrArray());
               delete $2;
           }
         | LPAREN elems RPAREN {
               $$ = new TableExprNode ($2->setOrArray());
               delete $2;
           }
         | LBRACKET selcomm RBRACKET {
	       TableParseSelect* p = TableParseSelect::popSelect();
               $$ = new TableExprNode (p->doSubQuery());
	       delete p;
           }
         | LPAREN selcomm RPAREN {
	       TableParseSelect* p = TableParseSelect::popSelect();
               $$ = new TableExprNode (p->doSubQuery());
	       delete p;
           }
         ;

elemlist:  elems
               { $$ = $1; }
         |
               { $$ = new TableExprNodeSet; }       /* no elements */
         ;

elems:     elems COMMA elem {
               $$ = $1;
	       $$->add (*$3);
	       delete $3;
	   }
         | elem {
	       $$ = new TableExprNodeSet;
	       $$->add (*$1);
	       delete $1;
	   }
         ;

elem:      orexpr {
               $$ = new TableExprNodeSetElem(*$1);
	       delete $1;
	   }
         | range {
               $$ = $1;
           }

singlerange: range {
	       TableExprNodeSet set;
	       set.add (*$1);
	       delete $1;
               $$ = new TableExprNode (set.setOrArray());
           }

range:     colonrange {
               $$ = $1;
           }
         | LT arithexpr COMMA arithexpr GT {
               $$ = new TableExprNodeSetElem (False, *$2, *$4, False);
	       delete $2;
	       delete $4;
           }
         | LT arithexpr COMMA arithexpr RBRACE {
               $$ = new TableExprNodeSetElem (False, *$2, *$4, True);
	       delete $2;
	       delete $4;
           }
         | LBRACE arithexpr COMMA arithexpr GT {
               $$ = new TableExprNodeSetElem (True, *$2, *$4, False);
	       delete $2;
	       delete $4;
           }
         | LBRACE arithexpr COMMA arithexpr RBRACE {
               $$ = new TableExprNodeSetElem (True, *$2, *$4, True);
	       delete $2;
	       delete $4;
           }
         | LBRACE COMMA arithexpr GT {
                $$ = new TableExprNodeSetElem (*$3, False);
	       delete $3;
          }
         | LT COMMA arithexpr GT {
                $$ = new TableExprNodeSetElem (*$3, False);
	       delete $3;
          }
         | LBRACE COMMA arithexpr RBRACE {
                $$ = new TableExprNodeSetElem (*$3, True);
	       delete $3;
           }
         | LT COMMA arithexpr RBRACE {
                $$ = new TableExprNodeSetElem (*$3, True);
	       delete $3;
           }
         | LT arithexpr COMMA RBRACE {
               $$ = new TableExprNodeSetElem (False, *$2);
	       delete $2;
           }
         | LT arithexpr COMMA GT {
               $$ = new TableExprNodeSetElem (False, *$2);
	       delete $2;
           }
         | LBRACE arithexpr COMMA RBRACE {
               $$ = new TableExprNodeSetElem (True, *$2);
	       delete $2;
           }
         | LBRACE arithexpr COMMA GT {
               $$ = new TableExprNodeSetElem (True, *$2);
	       delete $2;
           }
         | arithexpr OPENOPEN arithexpr {
               $$ = new TableExprNodeSetElem (False, *$1, *$3, False);
	       delete $1;
	       delete $3;
           }
         | arithexpr OPENCLOSED arithexpr {
               $$ = new TableExprNodeSetElem (False, *$1, *$3, True);
	       delete $1;
	       delete $3;
           }
         | arithexpr CLOSEDOPEN arithexpr {
               $$ = new TableExprNodeSetElem (True, *$1, *$3, False);
	       delete $1;
	       delete $3;
           }
         | arithexpr CLOSEDCLOSED arithexpr {
               $$ = new TableExprNodeSetElem (True, *$1, *$3, True);
	       delete $1;
	       delete $3;
           }
	 | EMPTYOPEN arithexpr {
               $$ = new TableExprNodeSetElem (*$2, False);
	       delete $2;
           }
	 | EMPTYCLOSED arithexpr {
               $$ = new TableExprNodeSetElem (*$2, True);
	       delete $2;
           }
	 | arithexpr OPENEMPTY {
               $$ = new TableExprNodeSetElem (False, *$1);
	       delete $1;
           }
	 | arithexpr CLOSEDEMPTY {
               $$ = new TableExprNodeSetElem (True, *$1);
	       delete $1;
           }
         ;

subscripts: subscripts COMMA subsrange {
               $$ = $1;
	       $$->add (*$3);
	       delete $3;
	   }
         | subscripts COMMA {
               $$ = $1;
	       $$->add (TableExprNodeSetElem (0, 0, 0));
	   }
         | COMMA {
	       $$ = new TableExprNodeSet;
	       $$->add (TableExprNodeSetElem (0, 0, 0));
	       $$->add (TableExprNodeSetElem (0, 0, 0));
	   }
         | COMMA subsrange {
	       $$ = new TableExprNodeSet;
	       $$->add (TableExprNodeSetElem (0, 0, 0));
	       $$->add (*$2);
	       delete $2;
	   }
         | subsrange {
	       $$ = new TableExprNodeSet;
	       $$->add (*$1);
	       delete $1;
	   }
         ;

subsrange: arithexpr {
               $$ = new TableExprNodeSetElem (*$1);
	       delete $1;
           }
         | colonrange {
               $$ = $1;
	   }
         ;

colonrange: arithexpr COLON arithexpr {
               $$ = new TableExprNodeSetElem ($1, $3, 0);
	       delete $1;
	       delete $3;
            }
         |  arithexpr COLON arithexpr COLON arithexpr {
               $$ = new TableExprNodeSetElem ($1, $3, $5);
	       delete $1;
	       delete $3;
	       delete $5;
            }
         |  arithexpr COLON {
	       TableExprNode incr(1);
               $$ = new TableExprNodeSetElem ($1, 0, &incr);
	       delete $1;
            }
         |  arithexpr COLON COLON arithexpr {
               $$ = new TableExprNodeSetElem ($1, 0, $4);
	       delete $1;
	       delete $4;
            }
         |  COLON arithexpr {
               $$ = new TableExprNodeSetElem (0, $2, 0);
	       delete $2;
            }
         |  COLON arithexpr COLON arithexpr {
               $$ = new TableExprNodeSetElem (0, $2, $4);
	       delete $2;
	       delete $4;
            }
         |  COLON COLON arithexpr {
               $$ = new TableExprNodeSetElem (0, 0, $3);
	       delete $3;
            }
         ;

sortlist : sortlist COMMA sortexpr {
               $$ = $1;
               $$->resize($$->nelements() + 1);
	       (*$$)[$$->nelements() - 1] = $3;
	   }
         | sortexpr {
	       $$ = new PtrBlock<TableParseSort*>(1);
	       (*$$)[0] = $1;
	   }
         ;

sortexpr : orexpr {
               $$ = new TableParseSort (*$1);
	       delete $1;
           }
         | orexpr SORTASC {
               $$ = new TableParseSort (*$1, Sort::Ascending);
	       delete $1;
           }
         | orexpr SORTDESC {
               $$ = new TableParseSort (*$1, Sort::Descending);
	       delete $1;
           }
         ;
%%
