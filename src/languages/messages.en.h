// This file is part of SmallBASIC
//
// SB Messages in English - Latin1
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2000 Nicholas Christopoulos

#define LOWER_CHAR_FOR_YES      'y'
#define WORD_FILE               "File"
#define WORD_SECTION            "Section"

#define TABLE_WEEKDAYS_3C    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" }
#define TABLE_WEEKDAYS_FULL  { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" }
#define TABLE_MONTH_3C       { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" }
#define TABLE_MONTH_FULL     { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" }

#define DATEFMT_DAY_U           'D'
#define DATEFMT_DAY_L           'd'
#define DATEFMT_MONTH_U         'M'
#define DATEFMT_MONTH_L         'm'
#define DATEFMT_YEAR_U          'Y'
#define DATEFMT_YEAR_L          'y'

#define RES_NUMBER_OF_VARS      "Number of variables %d (%d)\n"
#define RES_NUMBER_OF_LABS      "Number of labels    %d\n"
#define RES_NUMBER_OF_UDPS      "Number of UDFs/UDPs %d\n"
#define RES_CODE_SIZE           "Code size           %d\n"
#define RES_IMPORTED_LIBS       "Imported libraries  %d\n"
#define RES_IMPORTED_SYMS       "Imported symbols    %d\n"
#define RES_EXPORTED_SYMS       "Exported symbols    %d\n"
#define RES_FINAL_SIZE          "Final size          %d\n"

// compiler
#define MSG_WRONG_PROCNAME      "Wrong procedure/function name: %s"
#define MSG_EXP_MIS_DQ          "Expression: Missing (\"), string remains open"
#define MSG_EXP_MIS_RP          "Expression: Missing \')\'"
#define MSG_EXP_MIS_LP          "Expression: Missing \'(\'"
#define MSG_LABEL_NOT_DEFINED   "Label '%s' is not defined"
#define MSG_WRONG_UNIT          "Unknown unit '%s'; use IMPORT\n"
#define MSG_NEW_LABEL           "%d: new label [%s], index %d\n"
#define MSG_NEW_UDP             "%d: new UDP/F [%s], index %d\n"
#define MSG_NEW_VAR             "%d: new VAR [%s], index %d\n"
#define MSG_WRONG_VARNAME       "Wrong variable name: %s"
#define MSG_EXP_GENERR          "Error on numeric expression"
#define MSG_BF_ARGERR           "built-in function %s: without parameters"
#define MSG_KEYWORD_DO_ERR      "Keyword DO not allowed here"
#define MSG_STATEMENT_ON_RIGHT  "Statement %s must be on the left side (the first keyword of the line)"
#define MSG_OPR_APPEND_ERR      "operator << not allowed here"
#define MSG_WRONG_OPR           "Unknown operator: '%c'"
#define MSG_PARNUM_LIMIT        "Paremeters limit: %d"
#define MSG_NIL_PAR_ERR         "'Empty' parameters are not allowed"
#define MSG_MISSING_CHAR        "Missing '%c'"
#define MSG_ARRAY_SE            "Array: syntax error"
#define MSG_ARRAY_MIS_RP        "Array: Missing ')', (left side of expression)"
#define MSG_ARRAY_MIS_LP        "Array: Missing '(', (left side of expression)"
#define MSG_OPT_UICS_ERR        "Syntax error: OPTION UICS {CHARS|PIXELS}"
#define MSG_OPTION_ERR          "OPTION: Unrecognized option '%s'"
#define MSG_IT_IS_KEYWORD       "%s: is keyword (left side of expression)"
#define MSG_USE_DECL            "Use DECLARE with SUB or FUNC keyword"
#define MSG_LET_MISSING_EQ      "LET/CONST/APPEND: Missing '='"
#define MSG_EXIT_ERR            "Use EXIT [FOR|LOOP|SUB|FUNC]"
#define MSG_UDP_ALREADY_EXISTS  "The SUB/FUNC %s is already defined"
#define MSG_MISSING_UDP_BODY    "FUNC/DEF it has an '=' on declaration, but I didn't found the body!"
#define MSG_UNIT_NAME_MISSING   "EXPORT: Unit name is not defined"
#define MSG_ON_GOTO_ERR         "ON x GOTO WHERE?"
#define MSG_ON_GOSUB_ERR        "ON x GOSUB WHERE?"
#define MSG_ON_NOTHING          "ON WHAT?"
#define MSG_FOR_NOTHING         "FOR: Missing '=' OR 'IN'"
#define MSG_FOR_COUNT_ERR       "FOR: %s is not a variable"
#define MSG_FOR_ARR_COUNT       "FOR: %s is an array. Arrays are not allowed"
#define MSG_SPECIAL_KW_ERR      "%s: Wrong position"
#define MSG_DETAILED_REPORT_Q   "Detailed report (y/N) ?"
#define MSG_PASS2_COUNT         "\rPASS2: Node %d/%d"
#define MSG_UDP_MISSING_END     "SUB/FUNC: Missing END on the same level"
#define MSG_MISSING_NEXT        "FOR: Missing NEXT on the same level"
#define MSG_MISSING_IN          "FOR: Missing IN"
#define MSG_MISSING_TO          "FOR: Missing TO"
#define MSG_MISSING_WEND        "WHILE: Missing WEND on the same level"
#define MSG_MISSING_UNTIL       "REPEAT: Missing UNTIL on the same level"
#define MSG_MISSING_ENDIF_OR_ELSE "IF: Missing ELIF/ELSE/ENDIF on the same level"
#define MSG_MISSING_ENDIF       "(ELSE) IF: Missing ENDIF on the same level"
#define MSG_MISSING_WHILE       "WEND: Missing WHILE on the same level"
#define MSG_MISSING_REPEAT      "UNTIL: Missing REPEAT on the same level"
#define MSG_MISSING_FOR         "NEXT: Missing FOR on the same level"
#define MSG_MISSING_IF          "ENDIF: Missing IF on the same level"
#define MSG_MEMBER_DOES_NOT_EXISTS "Unit has no member named '%s'\n"
#define MSG_CANT_OPEN_FILE_AT   "Can't open '%s' at '%s'\n"
#define MSG_CANT_OPEN_FILE      "Can't open '%s'\n"
#define MSG_GRMODE_ERR          "GRMODE, usage:<width>x<height>[x<bits-per-pixel>]\nExample: OPTION PREDEF GRMODE=640x480x4\n"
#define MSG_UNIT_NOT_FOUND      "Unit %s.sbu not found"
#define MSG_IMPORT_FAILED       "Unit %s.sbu, import failed"
#define MSG_INVALID_UNIT_NAME   "Invalid unit name"
#define MSG_UNIT_ALREADY_DEFINED "Unit name already defined"
#define MSG_OPT_PREDEF_ERR      "OPTION PREDEF: Unrecognized option '%s'"
#define MSG_MANY_UNIT_DECL      "Use 'Unit' keyword only once"
#define MSG_INC_MIS_DQ          "#INC: Missing \""
#define MSG_INC_FILE_DNE        "File %s: File %s does not exist"
#define MSG_INC_FILE_INC        "File %s: File cannot include itself"
#define MSG_UDP_ALREADY_DECL    "SUB/FUNC %s already defined"
#define MSG_UDP_MIS_END_2       "File %s: SUB/FUNC: Missing END (possibly in %s)"
#define MSG_PASS1               "Pass1...\n"
#define MSG_PASS1_COUNT         "\rPASS1: Line %d"
#define MSG_UNDEFINED_UDP       "Undefined SUB/FUNC code: %s"
#define MSG_UNDEFINED_HASH      "Undefined HASH: %s, (Use DIM)"
#define MSG_PASS1_FIN           "\rPASS1: Line %d; finished\n"
#define MSG_EXP_SYM_NOT_FOUND   "Export symbol '%s' not found"
#define MSG_PASS2               "PASS2..."
#define MSG_MISSING_END_3       "SUB/FUNC: Missing END"
#define MSG_LOOPS_OPEN          "%d loop(s) remains open"
#define MSG_CREATING_UNIT       "\nCreating Unit %s...\n"
#define MSG_CREATING_BC         "Creating byte-code...\n"
#define MSG_UNIT_NAME_DIF_THAN_SRC  "Warning: unit's file name is different than source\n"
#define MSG_BC_FILE_CREATED         "ByteCode file '%s' created!\n"
#define MSG_BC_FILE_ERROR           "Can't create binary file\n"
#define MSG_CASE_CASE_ELSE      "CASE following CASE ELSE"
#define MSG_MISSING_END_SELECT  "SELECT/CASE without 'END SELECT'"
#define MSG_MISSING_SELECT      "END SELECT without SELECT"
#define MSG_MISSING_CASE        "SELECT without CASE"

// executor
#define WORD_ERROR_AT           "ERROR AT"
#define WORD_DESCRIPTION        "Description"
#define WORD_COMP               "COMP"  // Compiler (COMP) error or RunTime error (RTE)
#define WORD_RTE                "RTE"
#define WORD_DONE               "DONE"
#define WORD_BREAK_AT           "BREAK AT LINE"
#define WORD_INPUT_REDO         "REDO FROM START"
#define WORD_GOTO               "GOTO"
#define WORD_GOSUB              "GOSUB"
#define WORD_INF                "INF" // infinity
#define FSERR_OUT_OF_MEMORY     "FS: Out of memory"
#define FSERR_INVALID_PARAMETER "FS: Invalid parameter"
#define FSERR_CORRUPTED         "FS: File is corrupted or invalid"
#define FSERR_NOT_FOUND         "FS: File not found"
#define FSERR_TYPE_MSM          "FS: Type or creator not what was specified"
#define FSERR_OVERWRITE         "FS: Coundn't replace existing file"
#define FSERR_CREATE            "FS: Couldn't create new file"
#define FSERR_OPEN              "FS: Generic open error"
#define FSERR_USED              "FS: File is in use" // (exclusive)
#define FSERR_READ_ONLY         "FS: File is read-only"
#define FSERR_HANDLE            "FS: Invalid file handle"
#define FSERR_CLOSE             "FS: Error closing file"
#define FSERR_EOF               "FS: Past end of file"
#define FSERR_ACCESS            "FS: Access denied"
#define FSERR_GENERIC           "FS: Generic I/O error"
#define FSERR_PALM_EOF          "FS: End-Of-File error!"
#define FSERR_ANOMALO           "FS: File is not a stream"
#define FSERR_FMT               "FS(%d): %s"
#define FSERR_TOO_MANY_FILES    "FS: Too many open files"
#define FSERR_WRONG_DRIVER      "Unknown device or file-system"
#define ERR_MISSING_RP          "Missing ')' OR invalid number of parameters"
#define ERR_MATRIX_DIM          "Matrix dimension error"
#define ERR_SYNTAX              "Syntax error"
#define ERR_MISSING_SEP         "Missing separator '%c'"
#define ERR_PARCOUNT            "Error number of parameters"
#define ERR_STACK_OVERFLOW      "Stack overflow"
#define ERR_STACK_UNDERFLOW     "Stack underflow"
#define ERR_STACK               "Stack mess (Cannot use call to SUB with an expression)"
#define ERR_ARRAY_MISSING_LP    "Array: Missing '('"
#define ERR_ARRAY_MISSING_RP    "Array: Missing ')'"
#define ERR_ARRAY_RANGE         "Array: Index '%d' out of range. (Max = %d)"
#define ERR_TYPE                "Type mismatch"
#define ERR_PARAM               "Invalid parameter"
#define EVAL_VAR_IS_ARRAY       "Expr/RT: Variable is an array"
#define EVAL_VAR_IS_NOT_ARRAY   "Expr/RT: Variable is NOT an array (Use DIM)"
#define EVAL_NOT_A_NUM          "Expr/RT: Not a number"
#define EVAL_SYNTAX             "Expr/RT: Syntax error"
#define EVAL_TYPE               "Expr/RT: Type mismatch"
#define EVAL_PARAM              "Expr/RT: Invalid parameter"
#define ERR_UNSUPPORTED         "Unsupported"
#define ERR_CONST               "LET: Cannot change a constant"
#define ERR_NOT_A_VAR           "Not a variable"
#define ERR_NOT_ARR_OR_FUNC     "NOT an array OR function"
#define ERR_RANGE               "Out of range"
#define ERR_MISSING_SEP_OR_PAR  "Missing separator OR parenthesis"
#define ERR_DIVZERO             "Division by zero"
#define ERR_OPERATOR            "Operator cannot be used with an array"
#define ERR_MATSIG              "Matrix singular"
#define ERR_MISSING_LP          "Missing '('"
#define ERR_PARFMT              "Parameters count/format error (%s)"
#define ERR_BYREF               "Parameter %d cannot BYREF"
#define ERR_STR_RANGE           "String: Index out of range (%d)"
#define ERR_BAD_FILE_HANDLE     "VFS: Bad file number (Use OPEN)"
#define ERR_SEP_FMT             "No separator found (missing %s)"
#define ERR_POLY                "Parsing polyline: type mismatch! (element: %d, info: %d)"
#define ERR_CRITICAL_MISSING_FUNC  "Unsupported built-in function call %ld, please report this bug"
#define ERR_GPF                 "\n\aOUT OF ADDRESS SPACE\n"
#define ERR_CRITICAL_MISSING_PROC  "Unsupported built-in procedure call %ld, please report this bug"
#define ERR_CHAIN_FILE          "CHAIN: File '%s' does not exist"
#define ERR_RUN_FILE            "RUN/EXEC\"%s\" Failed"
#define ERR_RUNFUNC_FILE        "RUN(\"%s\"): Failed"
#define ERR_PARCOUNT_SP         "Parameter count error @%d=%X"
#define ERR_DATE                "Invalid DATE: '%s'. Expected DD/MM/YYYY"
#define ERR_TIME                "Invalid TIME: '%s'. Expected HH:MM:SS"
#define ERR_BOUND_DIM           "U/LBOUND: Array of %d dimensions (%d)"
#define ERR_BOUND_VAR           "U/LBOUND: Variable is not an array"
#define ERR_FORMAT_INVALID_FORMAT "Wrong format"
#define ERR_CENTROID            "Centroid is undefined"
#define ERR_WRONG_POLY          "Polygon is wrong"
#define ERR_POINT               "POINT() Invalid parameter"
#define ERR_LINEEQN_ADIM        "MATRIX A, WRONG DIM %dx%d"
#define ERR_LINEEQN_BDIM        "MATRIX B, WRONG DIM %dx%d"
#define ERR_WRONG_MAT           "MATRIX: WRONG DIM %dx%d"
#define ERR_INPUT_NO_VARS       "INPUT without variables"
#define ERR_ONGOTO_RANGE        "ON x %s: Out of range"
#define ERR_EXITFOR             "EXIT FOR: No 'FOR' inside subroutine or function"
#define ERR_EXITLOOP            "EXIT LOOP: No 'LOOP' inside subroutine or function"
#define ERR_READ_DATA_START     "READ: Data out of range. Use RESTORE [label]"
#define ERR_READ_DATA_INDEX     "READ: Data out of range. Use RESTORE [label]"
#define ERR_READ_DATA_INDEX_FMT "READ: Data out of range (IDX=%d). Use RESTORE [label]"
#define ERR_PUTENV              "ENV failed"
#define ERR_EXPRSEQ_WITHOUT_EXPR "EXPRSEQ: Missing expression"
#define ERR_POLY_POINT          "Parsing point: type mismatch!"
#define ERR_VP_POS              "Viewport out of screen"
#define ERR_VP_ZERO             "Viewport of zero size"
#define ERR_WIN_ZERO            "Window of zero size"
#define ERR_DRAW_SEP            "DRAW: Missing separator (,)"
#define ERR_DRAW_CMD            "DRAW: Command '%c' unsupported"
#define ERR_FILE_PERM           "File IO permission error"

// memory manager
#define MEM_OUT_OF_MEM          "OUT OF MEMORY"

