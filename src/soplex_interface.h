
#ifdef __cplusplus
extern "C" {
#endif

/** creates new SoPlex struct **/
void* SoPlex_create();

/** frees SoPlex struct **/
void SoPlex_free(void* soplex);

/** clears the (floating point) LP **/
void SoPlex_clearLPReal(void* soplex);

/** returns number of rows **/
int SoPlex_numRows(void* soplex);

/** returns number of columns **/
int SoPlex_numCols(void* soplex);

/** enables rational solving mode  **/
void SoPlex_setRational(void* soplex);

/** sets integer parameter value **/
void SoPlex_setIntParam(void* soplex, int paramcode, int paramvalue);

/** returns value of integer parameter **/
int SoPlex_getIntParam(void* soplex, int paramcode);

/** adds a single (floating point) column **/
void SoPlex_addColReal(
   void* soplex,
   double* colentries,
   int colsize,
   int nnonzeros,
   double objval,
   double lb,
   double ub
);

/** adds a single rational column **/
void SoPlex_addColRational(
   void* soplex,
   long* colnums,
   long* coldenoms,
   int colsize,
   int nnonzeros,
   long objvalnum,
   long objvaldenom,
   long lbnum,
   long lbdenom,
   long ubnum,
   long ubdenom
);

/** adds a single (floating point) column **/
void SoPlex_addRowReal(
   void* soplex,
   double* rowentries,
   int rowsize,
   int nnonzeros,
   double lb,
   double ub
);

/** adds a single rational row **/
void SoPlex_addRowRational(
   void* soplex,
   long* rownums,
   long* rowdenoms,
   int rowsize,
   int nnonzeros,
   long lbnum,
   long lbdenom,
   long ubnum,
   long ubdenom
);

/** gets primal solution **/
void SoPlex_getPrimalReal(void* soplex, double* primal, int dim);

/** Returns rational primal solution in a char pointer.
*   The caller needs to ensure the char array is freed.
**/
char* SoPlex_getPrimalRationalString(void* soplex, int dim);

/** gets dual solution **/
void SoPlex_getDualReal(void* soplex, double* dual, int dim);

/** optimizes the given LP **/
int SoPlex_optimize(void* soplex);

/** changes objective function vector to obj **/
void SoPlex_changeObjReal(void* soplex, double* obj, int dim);

/** changes rational objective function vector to obj **/
void SoPlex_changeObjRational(void* soplex, long* objnums, long* objdenoms, int dim);

/** changes left-hand side vector for constraints to lhs **/
void SoPlex_changeLhsReal(void* soplex, double* lhs, int dim);

/** changes rational left-hand side vector for constraints to lhs **/
void SoPlex_changeLhsRational(void* soplex, long* lhsnums, long* lhsdenoms, int dim);

/** changes right-hand side vector for constraints to rhs **/
void SoPlex_changeRhsReal(void* soplex, double* rhs, int dim);

/** changes rational right-hand side vector for constraints to rhs **/
void SoPlex_changeRhsRational(void* soplex, long* rhsnums, long* rhsdenoms, int dim);

/** write LP to file **/
void SoPlex_writeFileReal(void* soplex, char* filename);

/** returns the objective value if a primal solution is available **/
double SoPlex_objValueReal(void* soplex);

/** Returns the rational objective value (as a string) if a primal solution is available.
*   The caller needs to ensure the char array is freed.
**/
char* SoPlex_objValueRationalString(void* soplex);

/** changes vectors of column bounds to lb and ub **/
void SoPlex_changeBoundsReal(void* soplex, double* lb, double* ub, int dim);

/** changes bounds of a column to lb and ub **/
void SoPlex_changeVarBoundsReal(void* soplex, int colidx, double lb, double ub);

/** changes rational bounds of a column to lbnum/lbdenom and ubnum/ubdenom **/
void SoPlex_changeVarBoundsRational(
   void* soplex,
   int colidx,
   long lbnum,
   long lbdenom,
   long ubnum,
   long ubdenom
);

/** changes upper bound of column to ub **/
void SoPlex_changeVarUpperReal(void* soplex, int colidx, double ub);

/** changes upper bound vector of columns to ub **/
void SoPlex_getUpperReal(void* soplex, double* ub, int dim);

/** returns status of row
 *  0 -> row is set to its upper bound
 *  1 -> row is set to its lower bound
 *  2 -> row is fixed to its identical bounds
 *  4 -> row is basic
 *  5 -> nothing known about basis status
 **/
int SoPlex_basisRowStatus(void* soplex, int rowidx);

/** returns status of column
 *  0 -> column is set to its upper bound
 *  1 -> column is set to its lower bound
 *  2 -> column is fixed to its identical bounds
 *  3 -> column is free and fixed to zero
 *  4 -> column is basic
 *  5 -> nothing known about basis status
 **/
int SoPlex_basisColStatus(void* soplex, int colidx);

/** get non-zero entries and indices of row i **/
void SoPlex_getRowVectorReal(
   void* soplex,
   int i,
   int* nnonzeros,
   long* indices,
   double* coefs
);

/** get non-zero entries and indices of rational row i **/
void SoPlex_getRowVectorRational(
   void* soplex,
   int i,
   int* nnonzeros,
   long* indices,
   long* coefsnum,
   long* coefsdenom
);

/** get lower and upper bounds of row i **/
void SoPlex_getRowBoundsReal(
   void* soplex,
   int i,
   double* lb,
   double* ub
);

/** get rational lower and upper bounds of row i **/
void SoPlex_getRowBoundsRational(
   void* soplex,
   int i,
   long* lbnum,
   long* lbdenom,
   long* ubnum,
   long* ubdenom
);

#ifdef __cplusplus
}
#endif
