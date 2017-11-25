#pragma once

#define MARCO_EXPAND(...)                 __VA_ARGS__
#define APPLY_VARIADIC_MACRO(macro, ...)  MARCO_EXPAND(macro(__VA_ARGS__))

#define VA_ARGS_NUM(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

/* arg list expand macro, now support 50 args */
#define MAKE_ARG_LIST_1(name, op, dl, arg, ...)   op(name, arg)
#define MAKE_ARG_LIST_2(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_1(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_3(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_2(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_4(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_3(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_5(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_4(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_6(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_5(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_7(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_6(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_8(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_7(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_9(name, op, dl, arg, ...)   op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_8(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_10(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_9(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_11(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_10(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_12(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_11(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_13(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_12(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_14(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_13(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_15(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_14(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_16(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_15(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_17(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_16(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_18(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_17(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_19(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_18(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_20(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_19(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_21(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_20(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_22(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_21(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_23(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_22(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_24(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_23(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_25(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_24(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_26(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_25(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_27(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_26(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_28(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_27(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_29(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_28(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_30(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_29(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_31(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_30(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_32(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_31(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_33(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_32(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_34(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_33(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_35(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_34(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_36(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_35(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_37(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_36(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_38(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_37(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_39(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_38(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_40(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_39(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_41(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_40(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_42(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_41(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_43(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_42(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_44(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_43(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_45(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_44(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_46(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_45(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_47(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_46(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_48(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_47(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_49(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_48(name, op, dl, __VA_ARGS__))
#define MAKE_ARG_LIST_50(name, op, dl, arg, ...)  op(name, arg) dl() MARCO_EXPAND(MAKE_ARG_LIST_49(name, op, dl, __VA_ARGS__))


/* emmbed marco, using EMMBED_TUPLE(5 , a, b, c, d, e) */
//note use MACRO_CONCAT like A##_##B direct may cause marco expand error
#define MACRO_CONCAT(A, B) MACRO_CONCAT1(A, B)
#define MACRO_CONCAT1(A, B) A##_##B

#define MAKE_ARG_LIST(name, N, op, delimiter, arg, ...) \
        MACRO_CONCAT(MAKE_ARG_LIST, N)(name, op, delimiter, arg, __VA_ARGS__)

#define COMMA_DELIMITER() ,
#define SEM_DELIMITER() ;


#define RSEQ_N() \
		 119,118,117,116,115,114,113,112,111,110,\
		 109,108,107,106,105,104,103,102,101,100,\
		 99,98,97,96,95,94,93,92,91,90, \
		 89,88,87,86,85,84,83,82,81,80, \
		 79,78,77,76,75,74,73,72,71,70, \
		 69,68,67,66,65,64,63,62,61,60, \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define ARG_N( \
         _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
		 _61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
		 _71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
		 _81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
		 _91,_92,_93,_94,_95,_96,_97,_98,_99,_100, \
		 _101,_102,_103,_104,_105,_106,_107,_108,_109,_110, \
         _111,_112,_113,_114,_115,_116,_117,_118,_119,N, ...) N

#define GET_ARG_COUNT_INNER(...)    MARCO_EXPAND(ARG_N(__VA_ARGS__))
#define GET_ARG_COUNT(...)          GET_ARG_COUNT_INNER(__VA_ARGS__, RSEQ_N())