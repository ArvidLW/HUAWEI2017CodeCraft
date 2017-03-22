#include "lpkit.h"
#include "lpglob.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Globals */
lprec   *Lp=NULL; /* pointer to active problem */
int     Rows;
int     Columns;
int     Sum;
int     Non_zeros;
int     Level;
matrec  *Mat;
int     *Col_no;
int     *Col_end;
int     *Row_end;
REAL    *Orig_rh;
REAL    *Rh;
REAL    *Rhs;
short   *Must_be_int;
REAL    *Orig_upbo;
REAL    *Orig_lowbo;
REAL    *Upbo;
REAL    *Lowbo;
int     *Bas;
short   *Basis;
short   *Lower;
int     Eta_alloc; 
int     Eta_size;           
REAL    *Eta_value;
int     *Eta_row_nr;
int     *Eta_col_end;
int     Num_inv;
REAL    *Solution;
REAL    *Best_solution;
REAL    Infinite;
REAL    Epsilon;
REAL    Epsb;
REAL    Epsd;
REAL    Epsel;

REAL	TREJ;
REAL	TINV;

short   Maximise;
short   Floor_first;
REAL    Extrad;

int     Warn_count; /* used in CHECK version of rounding macro */

void error(char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  fputc('\n', stderr);
  va_end(ap);
  exit(FAIL);
}

lprec *make_lp(int rows, int columns)
{
  lprec *newlp;
  int i, sum;  

  sum=rows+columns;
  if(rows < 0 || columns < 0)
    error("rows < 0 or columns < 0");
  CALLOC(newlp, 1, lprec);

  strcpy(newlp->lp_name, "unnamed");
  newlp->active=FALSE;
  newlp->verbose=FALSE;
  newlp->print_duals=FALSE;
  newlp->print_sol=FALSE;
  newlp->debug=FALSE;
  newlp->print_at_invert=FALSE;
  newlp->trace=FALSE;

  newlp->rows=rows;
  newlp->columns=columns;
  newlp->sum=sum;
  newlp->rows_alloc=rows;
  newlp->columns_alloc=columns;
  newlp->sum_alloc=sum;
  newlp->names_used=FALSE;

  newlp->obj_bound=DEF_INFINITE;
  newlp->infinite=DEF_INFINITE;
  newlp->epsilon=DEF_EPSILON;
  newlp->epsb=DEF_EPSB;
  newlp->epsd=DEF_EPSD;
  newlp->epsel=DEF_EPSEL;
  newlp->non_zeros=0;
  newlp->mat_alloc=1;
  CALLOC(newlp->mat, newlp->mat_alloc, matrec);
  CALLOC(newlp->col_no, newlp->mat_alloc, int);
  CALLOC(newlp->col_end, columns + 1, int);
  CALLOC(newlp->row_end, rows + 1, int);
  newlp->row_end_valid=FALSE;
  CALLOC(newlp->orig_rh, rows + 1, REAL);
  CALLOC(newlp->rh, rows + 1, REAL);
  CALLOC(newlp->rhs, rows + 1, REAL);
  CALLOC(newlp->must_be_int, sum + 1, short);
  for(i = 0; i <= sum; i++)
    newlp->must_be_int[i]=FALSE;
  CALLOC(newlp->orig_upbo, sum + 1, REAL);
  for(i = 0; i <= sum; i++)
    newlp->orig_upbo[i]=newlp->infinite;
  CALLOC(newlp->upbo, sum + 1, REAL);
  CALLOC(newlp->orig_lowbo, sum + 1, REAL);
  CALLOC(newlp->lowbo, sum + 1, REAL);

  newlp->basis_valid=TRUE;
  CALLOC(newlp->bas, rows+1, int);
  CALLOC(newlp->basis, sum + 1, short);
  CALLOC(newlp->lower, sum + 1, short);
  for(i = 0; i <= rows; i++)
    {
      newlp->bas[i]=i;
      newlp->basis[i]=TRUE;
    }
  for(i = rows + 1; i <= sum; i++)
    newlp->basis[i]=FALSE;
  for(i = 0 ; i <= sum; i++)
    newlp->lower[i]=TRUE;
 
  newlp->eta_valid=TRUE;
  newlp->eta_size=0;
  newlp->eta_alloc=10000;
  newlp->max_num_inv=DEFNUMINV;

  newlp->nr_lagrange=0;

  CALLOC(newlp->eta_value, newlp->eta_alloc, REAL);
  CALLOC(newlp->eta_row_nr, newlp->eta_alloc, int);
  CALLOC(newlp->eta_col_end, newlp->rows_alloc + newlp->max_num_inv, int);

  newlp->bb_rule=FIRST_NI;
  newlp->break_at_int=FALSE;
  newlp->break_value=0;

  newlp->iter=0;
  newlp->total_iter=0;
  CALLOC(newlp->solution, sum + 1, REAL);
  CALLOC(newlp->best_solution, sum + 1, REAL);
  CALLOC(newlp->duals, rows + 1, REAL);

  newlp->maximise = FALSE;
  newlp->floor_first = TRUE;

  newlp->scaling_used = FALSE;
  newlp->columns_scaled = FALSE;

  CALLOC(newlp->ch_sign, rows + 1, short);

  for(i = 0; i <= rows; i++)
    newlp->ch_sign[i] = FALSE;

  newlp->valid = FALSE; 

  return(newlp);
}

void delete_lp(lprec *lp)
{
  int i; 

  if(lp->active)
    Lp=NULL;
  if(lp->names_used)
    {
      free(lp->row_name);
      free(lp->col_name);
    }
  free(lp->mat);
  free(lp->col_no);
  free(lp->col_end);
  free(lp->row_end);
  free(lp->orig_rh);
  free(lp->rh);
  free(lp->rhs);
  free(lp->must_be_int);
  free(lp->orig_upbo);
  free(lp->orig_lowbo);
  free(lp->upbo);
  free(lp->lowbo);
  free(lp->bas);
  free(lp->basis);
  free(lp->lower);
  free(lp->eta_value);
  free(lp->eta_row_nr);
  free(lp->eta_col_end);
  free(lp->solution);
  free(lp->best_solution);
  free(lp->duals);
  free(lp->ch_sign);
  if(lp->scaling_used)
    free(lp->scale);
  if(lp->nr_lagrange>0)
    {
      free(lp->lag_rhs);
      free(lp->lambda);
      free(lp->lag_con_type);
      for(i=0; i < lp->nr_lagrange; i++)
        free(lp->lag_row[i]);
      free(lp->lag_row);
    }  

  free(lp);
  lp=NULL;
}

void inc_mat_space(lprec *lp, int maxextra)
{
   if(lp->non_zeros + maxextra >= lp->mat_alloc)
     {
       lp->mat_alloc = lp->non_zeros + maxextra;
       REALLOC(lp->mat, lp->mat_alloc, matrec);
       REALLOC(lp->col_no, lp->mat_alloc, int);
       if (lp->active)
         {
           Mat=lp->mat;
           Col_no=lp->col_no;
         }
     }
}
 
void inc_row_space(lprec *lp)
{
  if(lp->rows > lp->rows_alloc)
    {
      lp->rows_alloc=lp->rows+10;
      lp->sum_alloc=lp->rows_alloc+lp->columns_alloc;
      REALLOC(lp->orig_rh, lp->rows_alloc + 1, REAL);
      REALLOC(lp->rh, lp->rows_alloc + 1, REAL);
      REALLOC(lp->rhs, lp->rows_alloc + 1, REAL);
      REALLOC(lp->orig_upbo, lp->sum_alloc + 1, REAL);
      REALLOC(lp->upbo, lp->sum_alloc + 1, REAL);
      REALLOC(lp->orig_lowbo, lp->sum_alloc + 1, REAL);
      REALLOC(lp->lowbo, lp->sum_alloc + 1, REAL);
      REALLOC(lp->solution, lp->sum_alloc + 1, REAL);
      REALLOC(lp->best_solution, lp->sum_alloc + 1,  REAL);
      REALLOC(lp->row_end, lp->rows_alloc + 1, int);
      REALLOC(lp->basis, lp->sum_alloc + 1, short);
      REALLOC(lp->lower, lp->sum_alloc + 1, short);
      REALLOC(lp->must_be_int, lp->sum_alloc + 1, short);
      REALLOC(lp->bas, lp->rows_alloc + 1, int);
      REALLOC(lp->duals, lp->rows_alloc + 1, REAL);
      REALLOC(lp->ch_sign, lp->rows_alloc + 1, short);
      REALLOC(lp->eta_col_end, lp->rows_alloc + lp->max_num_inv, int);
      if(lp->names_used)
        REALLOC(lp->row_name, lp->rows_alloc + 1, nstring);
      if(lp->scaling_used)
        REALLOC(lp->scale, lp->sum_alloc + 1, REAL);
      if(lp->active)
        set_globals(lp); 
    }
}

void set_mat(lprec *lp, int Row, int Column, REAL Value)
{
  int elmnr, lastelm, i;

  if(Row > lp->rows || Row < 0)
    error("Row out of range");
  if(Column > lp->columns || Column < 1)
    error("Column out of range");
  if(lp->scaling_used)
    Value *= lp->scale[Row] * lp->scale[lp->rows + Column];
  
   if(TRUE /*abs(Value) > lp->epsilon*/)
    {
       if (lp->basis[Column] == TRUE && Row > 0)
         lp->basis_valid = FALSE;
       lp->eta_valid = FALSE;
       elmnr = lp->col_end[Column-1];
       while((elmnr < lp->col_end[Column]) ?
	     (lp->mat[elmnr].row_nr != Row) : FALSE)
         elmnr++;

       if((elmnr != lp->col_end[Column]) ?
	  (lp->mat[elmnr].row_nr == Row) : FALSE )
         if (lp->scaling_used)
           {
             if(lp->ch_sign[Row])
               lp->mat[elmnr].value = 
		 -Value * lp->scale[Row] * lp->scale[Column];
             else
               lp->mat[elmnr].value =
		 Value * lp->scale[Row] * lp->scale[Column];
           }
         else
           {
             if(lp->ch_sign[Row])
               lp->mat[elmnr].value = -Value;
             else
               lp->mat[elmnr].value = Value;
           }
       else
         {
           /* check if more space is needed for matrix */
           inc_mat_space(lp,1);

           /* Shift the matrix */
           lastelm=lp->non_zeros; 
           for(i = lastelm; i > elmnr ; i--)
             lp->mat[i]=lp->mat[i-1];
           for(i = Column; i <= lp->columns; i++)
             lp->col_end[i]++;

           /* Set new element */
           lp->mat[elmnr].row_nr=Row;

           if (lp->scaling_used)
             {
               if(lp->ch_sign[Row])
                 lp->mat[elmnr].value=-Value*lp->scale[Row]*lp->scale[Column];
               else
                 lp->mat[elmnr].value=Value*lp->scale[Row]*lp->scale[Column];
             }
           else
             {
               if(lp->ch_sign[Row])
                  lp->mat[elmnr].value=-Value;
               else
                 lp->mat[elmnr].value=Value;
             }

           lp->row_end_valid=FALSE;
            
           lp->non_zeros++;
           if (lp->active)
             Non_zeros=lp->non_zeros;
        }      
    }
}

void set_obj_fn(lprec *lp, REAL *row)
{
  int i;
  for(i = 1; i <= lp->columns; i++)
    set_mat(lp, 0, i, row[i]);
}


void add_constraint(lprec *lp, REAL *row, short constr_type, REAL rh)
{
  matrec *newmat;
  int  i, j;
  int  elmnr;
  int  stcol;
  int  *addtoo;

  MALLOC(addtoo, lp->columns + 1, int)  

    for(i = 1; i <= lp->columns; i++)
      if(row[i]!=0)
	{
	  addtoo[i]=TRUE;
	  lp->non_zeros++;
	}
      else
	addtoo[i]=FALSE;

  MALLOC(newmat, lp->non_zeros, matrec);
  inc_mat_space(lp, 0);
  lp->rows++;
  lp->sum++;
  inc_row_space(lp);

  if(lp->scaling_used)
    {
      /* shift scale */
      for(i=lp->sum; i > lp->rows; i--)
        lp->scale[i]=lp->scale[i-1];
      lp->scale[lp->rows]=1;
    }

  if(lp->names_used)
    sprintf(lp->row_name[lp->rows], "r_%d", lp->rows);

  if(lp->scaling_used && lp->columns_scaled)
    for(i = 1; i <= lp->columns; i++)
      row[i] *= lp->scale[lp->rows+i];
     
  if(constr_type==GE)
    lp->ch_sign[lp->rows] = TRUE;
  else
    lp->ch_sign[lp->rows] = FALSE;

  elmnr = 0;
  stcol = 0;
  for(i = 1; i <= lp->columns; i++)
    {
      for(j = stcol; j < lp->col_end[i]; j++)
        {  
	  newmat[elmnr].row_nr=lp->mat[j].row_nr;
	  newmat[elmnr].value=lp->mat[j].value;
	  elmnr++;
        }
      if(addtoo[i])
        {
	  if(lp->ch_sign[lp->rows])
	    newmat[elmnr].value = -row[i];
	  else
	    newmat[elmnr].value = row[i];
	  newmat[elmnr].row_nr = lp->rows;
	  elmnr++;
        }
      stcol=lp->col_end[i];
      lp->col_end[i]=elmnr;
    }    
  
  memcpy(lp->mat, newmat, lp->non_zeros*sizeof(matrec));
 
  free(newmat);
  free(addtoo);

  for(i=lp->sum ; i > lp->rows; i--)
    {
      lp->orig_upbo[i]=lp->orig_upbo[i-1];
      lp->orig_lowbo[i]=lp->orig_lowbo[i-1];
      lp->basis[i]=lp->basis[i-1];
      lp->lower[i]=lp->lower[i-1];
      lp->must_be_int[i]=lp->must_be_int[i-1];
    }

  for(i= 1 ; i <= lp->rows; i++)
    if(lp->bas[i] >= lp->rows)
      lp->bas[i]++;

  if(constr_type==LE || constr_type==GE)
    {
      lp->orig_upbo[lp->rows]=lp->infinite;
    }
  else if(constr_type==EQ)
    {
      lp->orig_upbo[lp->rows]=0;
    }
  else
    {
      fprintf(stderr, "Wrong constraint type\n");
      exit(FAIL);
    }

  lp->orig_lowbo[lp->rows]=0;

  if(constr_type==GE && rh != 0)
    lp->orig_rh[lp->rows]=-rh;
  else
    lp->orig_rh[lp->rows]=rh;  

  lp->row_end_valid=FALSE;
 
  lp->bas[lp->rows]=lp->rows;
  lp->basis[lp->rows]=TRUE;
  lp->lower[lp->rows]=TRUE;   
 
  if(lp->active)
    set_globals(lp);
  lp->eta_valid=FALSE;
}


void set_upbo(lprec *lp, int column, REAL value)
{
  if(column > lp->columns || column < 1)
    error("Column out of range");
  if(lp->scaling_used)
    value /= lp->scale[lp->rows + column];
  if(value < lp->orig_lowbo[lp->rows + column])
    error("Upperbound must be >= lowerbound"); 
  lp->eta_valid = FALSE;
  lp->orig_upbo[lp->rows+column] = value;
}

void set_lowbo(lprec *lp, int column, REAL value)
{
  if(column > lp->columns || column < 1)
    error("Column out of range");
  if(lp->scaling_used)
    value /= lp->scale[lp->rows + column];
  if(value > lp->orig_upbo[lp->rows + column])
    error("Upperbound must be >= lowerbound"); 
  lp->eta_valid = FALSE;
  lp->orig_lowbo[lp->rows+column] = value;
}

void set_int(lprec *lp, int column, short must_be_int)
{
  if(column > lp->columns || column < 1)
    error("Column out of range");
  lp->must_be_int[lp->rows+column]=must_be_int;
  if(must_be_int==TRUE)
    if(lp->columns_scaled)
      unscale_columns(lp);
}


void set_maxim(lprec *lp)
{
  int i;
  if(lp->maximise==FALSE)
    {
      for(i = 0; i < lp->non_zeros; i++)
	if(lp->mat[i].row_nr==0)
	  lp->mat[i].value*=-1;
      lp->eta_valid=FALSE;
    } 
  lp->maximise=TRUE;
  lp->ch_sign[0]=TRUE;
  if(lp->active)
    Maximise=TRUE;
}

void set_minim(lprec *lp)
{
  int i;
  if(lp->maximise==TRUE)
    {
      for(i = 0; i < lp->non_zeros; i++)
	if(lp->mat[i].row_nr==0)
	  lp->mat[i].value = -lp->mat[i].value;
      lp->eta_valid=FALSE;
    } 
  lp->maximise=FALSE;
  lp->ch_sign[0]=FALSE;
  if(lp->active)
    Maximise=FALSE;
}


void get_row(lprec *lp, int row_nr, REAL *row)
{
  int i, j;

  if(row_nr <0 || row_nr > lp->rows)
    error("Row nr. out of range in get_row");
  for(i = 1; i <= lp->columns; i++)
    {
      row[i]=0;
      for(j=lp->col_end[i-1]; j < lp->col_end[i]; j++)
        if(lp->mat[j].row_nr==row_nr)
          row[i]=lp->mat[j].value;
      if(lp->scaling_used)
        row[i] /= lp->scale[lp->rows+i] * lp->scale[row_nr];
    }
  if(lp->ch_sign[row_nr])
    for(i=0; i <= lp->columns; i++)
      if(row[i]!=0)
        row[i] = -row[i];
}



void print_lp(lprec *lp)
{
  int i, j;
  REAL *fatmat;
  CALLOC(fatmat, (lp->rows + 1) * lp->columns, REAL);
  for(i = 1; i <= lp->columns; i++)
    for(j = lp->col_end[i-1]; j < lp->col_end[i]; j++)
      fatmat[(i - 1) * (lp->rows + 1) + lp->mat[j].row_nr]=lp->mat[j].value;

  printf("problem name: %s\n", lp->lp_name);
  printf("          ");
  for(j = 1; j <= lp->columns; j++)
    if(lp->names_used)
      printf("%8s ", lp->col_name[j]);
    else
      printf("Var[%3d] ", j);
  if(lp->maximise)
    {
      printf("\nMaximise  ");
      for(j = 0; j < lp->columns; j++)
	printf("% 8.2f ",-fatmat[j*(lp->rows+1)]);
    }
  else
    {
      printf("\nMinimize  ");
      for(j = 0; j < lp->columns; j++)
	printf("% 8.2f ", fatmat[j*(lp->rows+1)]);
    }
  printf("\n");
  for(i = 1; i <= lp->rows; i++)
    {
      if(lp->names_used)
	printf("%9s ", lp->row_name[i]);
      else
	printf("Row[%3d]  ", i);
      for(j = 0; j < lp->columns; j++)
	if(lp->ch_sign[i] && fatmat[j*(lp->rows+1)+i] != 0)
	  printf("% 8.2f ",-fatmat[j*(lp->rows+1)+i]);
	else
	  printf("% 8.2f ", fatmat[j*(lp->rows+1)+i]);
      if(lp->orig_upbo[i]==lp->infinite)
	if(lp->ch_sign[i])
	  printf(">= ");
	else
	  printf("<= ");
      else
	printf(" = ");
      if(lp->ch_sign[i])
	printf("% 8.2f\n",-lp->orig_rh[i]);
      else
	printf("% 8.2f\n", lp->orig_rh[i]);
    }
  printf("Type      ");
  for(i = 1; i <= lp->columns; i++)
    if(lp->must_be_int[lp->rows+i]==TRUE)
      printf("     Int ");
    else
      printf("    Real ");
  printf("\nupbo      ");
  for(i = 1; i <= lp->columns; i++)
    if(lp->orig_upbo[lp->rows+i]==lp->infinite)
      printf("     Inf ");
    else
      printf("% 8.2f ", lp->orig_upbo[lp->rows+i]);
  printf("\nlowbo     ");
  for(i = 1; i <= lp->columns; i++)
    printf("% 8.2f ", lp->orig_lowbo[lp->rows+i]);
  printf("\n");
  for(i = 0; i < lp->nr_lagrange; i++)
    {
      printf("lag[%3d]  ", i);
      for(j = 1; j <= lp->columns; j++)
	printf("% 8.2f ", lp->lag_row[i][j]);
      if(lp->orig_upbo[i]==lp->infinite)
	if(lp->lag_con_type[i] == GE)
	  printf(">= ");
	else if(lp->lag_con_type[i] == LE)
	  printf("<= ");
	else if(lp->lag_con_type[i] == EQ)
	  printf(" = ");
      printf("% 8.2f\n", lp->lag_rhs[i]);
    }

  free(fatmat);
}  


static REAL minmax_to_scale(REAL min, REAL max)
{
  REAL scale;

  /* should do something sensible when min or max is 0, MB */
  if((min == 0) || (max == 0))
    return((REAL)1);

  scale = 1 / pow(10, (log10(min) + log10(max)) / 2);
  return(scale);
}

void unscale_columns(lprec *lp)
{
  int i, j;

  /* unscale mat */
  for(j = 1; j <= lp->columns; j++)
    for(i = lp->col_end[j - 1]; i < lp->col_end[j]; i++)
      lp->mat[i].value /= lp->scale[lp->rows + j];

  /* unscale bounds as well */
  for(i = lp->rows + 1; i < lp->sum; i++)
    {
      if(lp->orig_lowbo[i] != 0)
	lp->orig_lowbo[i] *= lp->scale[i];
      if(lp->orig_upbo[i] != lp->infinite)
	lp->orig_upbo[i] *= lp->scale[i];
    }
    
  for(i=lp->rows+1; i<= lp->sum; i++)
    lp->scale[i]=1;
  lp->columns_scaled=FALSE;
  lp->eta_valid=FALSE;
}


void reset_basis(lprec *lp)
{
  lp->basis_valid=FALSE;
}

void print_solution(lprec *lp)
{
  int i;

  fprintf(stdout, "Value of objective function: %16.10g\n",
	  (double)lp->best_solution[0]);

  /* print normal variables */
  for(i = 1; i <= lp->columns; i++)
    if(lp->names_used)
      fprintf(stdout, "%-10s%16.5g\n", lp->col_name[i],
	      (double)lp->best_solution[lp->rows+i]);
    else
      fprintf(stdout, "Var [%4d]  %16.5g\n", i,
	      (double)lp->best_solution[lp->rows+i]);

  /* print achieved constraint values */
  if(lp->verbose)
    {
      fprintf(stdout, "\nActual values of the constraints:\n");
      for(i = 1; i <= lp->rows; i++)
        if(lp->names_used)
	  fprintf(stdout, "%-10s%16.5g\n", lp->row_name[i],
		  (double)lp->best_solution[i]);
        else
          fprintf(stdout, "Row [%4d]  %16.5g\n", i,
		  (double)lp->best_solution[i]);  
    }

  if((lp->verbose || lp->print_duals))
    {
      if(lp->max_level != 1)
        fprintf(stdout,
		"These are the duals from the node that gave the optimal solution.\n");
      else
        fprintf(stdout, "\nDual values:\n");
      for(i = 1; i <= lp->rows; i++)
        if(lp->names_used)
	  fprintf(stdout, "%-10s%16.5g\n", lp->row_name[i],
		  (double)lp->duals[i]);
        else
          fprintf(stdout, "Row [%4d]  %16.5g\n", i, (double)lp->duals[i]); 
    }
} /* Printsolution */


void print_duals(lprec *lp)
{
  int i;
  for(i = 1; i <= lp->rows; i++)
    if(lp->names_used)
      fprintf(stdout, "%10s [%3d] % 10.4f\n", lp->row_name[i], i,
	      lp->duals[i]);
    else
      fprintf(stdout, "Dual       [%3d] % 10.4f\n", i, lp->duals[i]);
}

void print_scales(lprec *lp)
{
  int i;
  if(lp->scaling_used)
    {
      for(i = 0; i <= lp->rows; i++)
        fprintf(stdout, "Row[%3d]    scaled at % 10.6f\n", i, lp->scale[i]);
      for(i = 1; i <= lp->columns; i++)
        fprintf(stdout, "Column[%3d] scaled at % 10.6f\n", i,
		lp->scale[lp->rows + i]);
    }
}
