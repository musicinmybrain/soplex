
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

#ifdef SOPLEX_WITH_GMP
/** enables rational solving mode  **/
void SoPlex_setRational(void* soplex);
#endif

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

#ifdef SOPLEX_WITH_GMP
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
#endif

/** adds a single (floating point) column **/
void SoPlex_addRowReal(
   void* soplex,
   double* rowentries,
   int rowsize,
   int nnonzeros,
   double lb,
   double ub
);

#ifdef SOPLEX_WITH_GMP
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
#endif

/** gets primal solution **/
void SoPlex_getPrimalReal(void* soplex, double* primal, int dim);

/** gets rational primal solution as a string **/
char* SoPlex_getPrimalRationalString(void* soplex, int dim);

/** gets dual solution **/
void SoPlex_getDualReal(void* soplex, double* dual, int dim);

/** optimizes the given LP **/
int SoPlex_optimize(void* soplex);

/** changes objective function vector to obj **/
void SoPlex_changeObjReal(void* soplex, double* obj, int dim);

#ifdef SOPLEX_WITH_GMP
/** changes rational objective function vector to obj **/
void SoPlex_changeObjRational(void* soplex, long* objnums, long* objdenoms, int dim);
#endif

/** changes left-hand side vector for constraints to lhs **/
void SoPlex_changeLhsReal(void* soplex, double* lhs, int dim);

#ifdef SOPLEX_WITH_GMP
/** changes rational left-hand side vector for constraints to lhs **/
void SoPlex_changeLhsRational(void* soplex, long* lhsnums, long* lhsdenoms, int dim);
#endif

/** changes right-hand side vector for constraints to rhs **/
void SoPlex_changeRhsReal(void* soplex, double* rhs, int dim);

#ifdef SOPLEX_WITH_GMP
/** changes rational right-hand side vector for constraints to rhs **/
void SoPlex_changeRhsRational(void* soplex, long* rhsnums, long* rhsdenoms, int dim);
#endif

/** write LP to file **/
void SoPlex_writeFileReal(void* soplex, char* filename);

/** returns the objective value if a primal solution is available **/
double SoPlex_objValueReal(void* soplex);

/** returns the rational objective value (as a string) if a primal solution is available **/
char* SoPlex_objValueRationalString(void* soplex);

/** changes vectors of column bounds to lb and ub **/
void SoPlex_changeBoundsReal(void* soplex, double* lb, double* ub, int dim);

/** changes bounds of a column to lb and ub **/
void SoPlex_changeVarBoundsReal(void* soplex, int colidx, double lb, double ub);

#ifdef SOPLEX_WITH_GMP
/** changes rational bounds of a column to lbnum/lbdenom and ubnum/ubdenom **/
void SoPlex_changeVarBoundsRational(
   void* soplex,
   int colidx,
   long lbnum,
   long lbdenom,
   long ubnum,
   long ubdenom
);
#endif

/** changes upper bound of column to ub **/
void SoPlex_changeVarUpperReal(void* soplex, int colidx, double ub);

/** changes upper bound vector of columns to ub **/
void SoPlex_getUpperReal(void* soplex, double* ub, int dim);

#ifdef __cplusplus
}
#endif
