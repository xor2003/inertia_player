#ifndef TASMRECOVER_IPLAY_MASM__STUBS_H__
#define TASMRECOVER_IPLAY_MASM__STUBS_H__

/* PLEASE DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE LOST! LOOK FOR README FOR DETAILS */

/* 
 *
 */

#include "asm.h"

struct Memory;
extern Memory m;

void mainproc(_offsets _i, _STATE* _state);
//namespace iplay_masm_ {


enum _offsets : int {
kbegin = 0x1001,
k_moduleread = 0x1111,
kloc_10006 = 0x1112,
kloc_1001f = 0x1113,
kloc_10028 = 0x1114,
kloc_1002d = 0x1115,
kloc_10033 = 0x1116,
kloc_1003d = 0x1117,
kloc_10040 = 0x1118,
kloc_10045 = 0x1119,
kloc_10049 = 0x111a,
kloc_10052 = 0x111b,
kloc_10064 = 0x111c,
kloc_1006b = 0x111d,
kloc_1007b = 0x111e,
kloc_10080 = 0x111f,
kloc_1008a = 0x1120,
kloc_10092 = 0x1121,
kloc_10099 = 0x1122,
k_lfreaderr = 0x1123,
k_mod_n_t_module = 0x1124,
k_mod_flt8_module = 0x1125,
kloc_10118 = 0x1126,
k_mod_tdz_module = 0x1127,
k_mod_chn_module = 0x1128,
kloc_10137 = 0x1129,
kloc_10152 = 0x112a,
klocret_10154 = 0x112b,
k_mod_ch_module = 0x112c,
k_mod_cd81_module = 0x112d,
k_mod_mk_module = 0x112e,
kloc_101a6 = 0x112f,
kloc_101b7 = 0x1130,
kloc_101f4 = 0x1131,
kloc_10213 = 0x1132,
k_mod_1021e = 0x1133,
kloc_10230 = 0x1134,
k_mod_1024a = 0x1135,
kloc_10254 = 0x1136,
kloc_102c1 = 0x1137,
kloc_102dc = 0x1138,
kloc_102df = 0x1139,
k_mod_102f5 = 0x113a,
kloc_102fe = 0x113b,
kloc_10307 = 0x113c,
k_mod_read_10311 = 0x113d,
kloc_10315 = 0x113e,
kloc_10345 = 0x113f,
kloc_1035c = 0x1140,
kloc_10365 = 0x1141,
kloc_1036c = 0x1142,
kloc_10399 = 0x1143,
kloc_103a8 = 0x1144,
kloc_103b9 = 0x1145,
k__2stm_module = 0x1146,
k_stm_module = 0x1147,
kloc_103ff = 0x1148,
kloc_10445 = 0x1149,
kloc_1044d = 0x114a,
kloc_10467 = 0x114b,
kloc_10487 = 0x114c,
kloc_104b6 = 0x114d,
kloc_104c7 = 0x114e,
kloc_104e6 = 0x114f,
kloc_104f2 = 0x1150,
kloc_104f9 = 0x1151,
kloc_1050c = 0x1152,
kloc_10513 = 0x1153,
kloc_1052e = 0x1154,
kloc_10544 = 0x1155,
kloc_10565 = 0x1156,
k_s3m_module = 0x1157,
kloc_105c7 = 0x1158,
kloc_105ff = 0x1159,
kloc_10618 = 0x115a,
kloc_1061e = 0x115b,
kloc_10628 = 0x115c,
kloc_1063d = 0x115d,
kloc_10640 = 0x115e,
kloc_10652 = 0x115f,
kloc_1065f = 0x1160,
kloc_10672 = 0x1161,
kloc_10680 = 0x1162,
kloc_106a3 = 0x1163,
kloc_106d8 = 0x1164,
kloc_106f8 = 0x1165,
kloc_10704 = 0x1166,
kloc_1070d = 0x1167,
kloc_10720 = 0x1168,
kloc_1074f = 0x1169,
kloc_10778 = 0x116a,
kloc_1078f = 0x116b,
kloc_1079a = 0x116c,
kloc_107ac = 0x116d,
kloc_107b4 = 0x116e,
kloc_107d2 = 0x116f,
kloc_107e0 = 0x1170,
kloc_10809 = 0x1171,
kloc_10811 = 0x1172,
kloc_10826 = 0x1173,
kloc_1082d = 0x1174,
kloc_1083e = 0x1175,
kloc_10880 = 0x1176,
kloc_10885 = 0x1177,
kloc_10887 = 0x1178,
kloc_1088d = 0x1179,
kloc_1088f = 0x117a,
kloc_1089c = 0x117b,
kloc_108a6 = 0x117c,
kloc_108b1 = 0x117d,
kloc_108c9 = 0x117e,
k_e669_module = 0x117f,
k__669_module = 0x1180,
kloc_10914 = 0x1181,
kloc_1095c = 0x1182,
kloc_1096f = 0x1183,
kloc_10993 = 0x1184,
kloc_109bd = 0x1185,
kloc_10a0f = 0x1186,
kloc_10a20 = 0x1187,
kloc_10a2d = 0x1188,
kloc_10a40 = 0x1189,
kloc_10a47 = 0x118a,
kloc_10a75 = 0x118b,
kloc_10a83 = 0x118c,
kloc_10aaa = 0x118d,
kloc_10aac = 0x118e,
k_mtm_module = 0x118f,
kloc_10b0f = 0x1190,
kloc_10b25 = 0x1191,
k_snd_off_chunk = 0x1192,
kloc_10b5a = 0x1193,
kloc_10b66 = 0x1194,
kloc_10bb5 = 0x1195,
kloc_10bc6 = 0x1196,
kloc_10c12 = 0x1197,
kloc_10c15 = 0x1198,
kloc_10c20 = 0x1199,
kloc_10c36 = 0x119a,
kloc_10c3f = 0x119b,
kloc_10c5a = 0x119c,
kloc_10c73 = 0x119d,
kloc_10c89 = 0x119e,
kloc_10c91 = 0x119f,
kloc_10c92 = 0x11a0,
kloc_10caa = 0x11a1,
k_psm_module = 0x11a2,
kloc_10d8c = 0x11a3,
kloc_10dc7 = 0x11a4,
kloc_10df0 = 0x11a5,
kloc_10e15 = 0x11a6,
kloc_10e19 = 0x11a7,
kloc_10e4c = 0x11a8,
kloc_10e68 = 0x11a9,
kloc_10e72 = 0x11aa,
kloc_10e8c = 0x11ab,
kloc_10e92 = 0x11ac,
kloc_10eb6 = 0x11ad,
kloc_10ebd = 0x11ae,
kloc_10ece = 0x11af,
kloc_10edd = 0x11b0,
kloc_10edf = 0x11b1,
kloc_10eec = 0x11b2,
kloc_10ef4 = 0x11b3,
kloc_10f11 = 0x11b4,
k_far_module = 0x11b5,
kloc_10f6a = 0x11b6,
kloc_10f80 = 0x11b7,
kloc_10fb0 = 0x11b8,
kloc_10fcf = 0x11b9,
kloc_10fd7 = 0x11ba,
kloc_10fef = 0x11bb,
kloc_10ffe = 0x11bc,
kloc_1100f = 0x11bd,
kloc_1102d = 0x11be,
kloc_11037 = 0x11bf,
kloc_11051 = 0x11c0,
kloc_11058 = 0x11c1,
kloc_11082 = 0x11c2,
kloc_11094 = 0x11c3,
kloc_110b8 = 0x11c4,
kloc_110cb = 0x11c5,
kloc_110cf = 0x11c6,
kloc_110d9 = 0x11c7,
kloc_110e4 = 0x11c8,
kloc_110e6 = 0x11c9,
kloc_110eb = 0x11ca,
kloc_110ef = 0x11cb,
kloc_110f3 = 0x11cc,
kloc_110fa = 0x11cd,
kloc_110fc = 0x11ce,
kloc_110ff = 0x11cf,
kloc_11120 = 0x11d0,
kloc_1113a = 0x11d1,
kloc_11149 = 0x11d2,
kloc_11150 = 0x11d3,
kloc_11152 = 0x11d4,
kloc_11181 = 0x11d5,
kloc_1118d = 0x11d6,
kloc_111ad = 0x11d7,
kloc_111b3 = 0x11d8,
kloc_111c6 = 0x11d9,
kloc_111db = 0x11da,
kloc_111e8 = 0x11db,
kloc_11204 = 0x11dc,
kloc_11217 = 0x11dd,
k_ult_module = 0x11de,
kloc_11256 = 0x11df,
kloc_11265 = 0x11e0,
kloc_1126f = 0x11e1,
kloc_112b4 = 0x11e2,
kloc_112c4 = 0x11e3,
kloc_112f1 = 0x11e4,
kloc_11316 = 0x11e5,
kloc_1131a = 0x11e6,
kloc_1132d = 0x11e7,
kloc_11348 = 0x11e8,
kloc_11359 = 0x11e9,
kloc_11365 = 0x11ea,
kloc_11382 = 0x11eb,
kloc_1138e = 0x11ec,
kloc_113c6 = 0x11ed,
kloc_113e2 = 0x11ee,
kloc_113f8 = 0x11ef,
kloc_113ff = 0x11f0,
kloc_11417 = 0x11f1,
kloc_11420 = 0x11f2,
kloc_11438 = 0x11f3,
kloc_11443 = 0x11f4,
kloc_1145a = 0x11f5,
kloc_11494 = 0x11f6,
kloc_1149c = 0x11f7,
kloc_114c0 = 0x11f8,
kloc_114df = 0x11f9,
k_ult_1150b = 0x11fa,
kloc_11520 = 0x11fb,
kloc_11523 = 0x11fc,
kloc_1152a = 0x11fd,
kloc_11531 = 0x11fe,
kloc_11539 = 0x11ff,
kloc_1154b = 0x1200,
k_ult_read = 0x1201,
klocret_11584 = 0x1202,
kloc_11585 = 0x1203,
k_inr_read_118b0 = 0x1204,
kloc_1191c = 0x1205,
kloc_11967 = 0x1206,
kloc_11991 = 0x1207,
kloc_11999 = 0x1208,
kloc_119af = 0x1209,
kloc_119b2 = 0x120a,
k_inr_read_119b7 = 0x120b,
k_inr_module = 0x120c,
kloc_11a39 = 0x120d,
kloc_11a81 = 0x120e,
kloc_11a96 = 0x120f,
kloc_11aaa = 0x1210,
kloc_11ac0 = 0x1211,
kloc_11af3 = 0x1212,
kloc_11b09 = 0x1213,
kloc_11b20 = 0x1214,
kloc_11b28 = 0x1215,
kloc_11b3d = 0x1216,
kloc_11b41 = 0x1217,
k_dosseek = 0x1218,
k_dosfread = 0x1219,
k_memalloc12k = 0x121a,
k_mem_reallocx = 0x121b,
ksub_11ba6 = 0x121c,
kloc_11bb2 = 0x121d,
kloc_11bbb = 0x121e,
kloc_11bbe = 0x121f,
kloc_11bc6 = 0x1220,
kloc_11bce = 0x1221,
kloc_11bd1 = 0x1222,
kloc_11be5 = 0x1223,
kloc_11bee = 0x1224,
kloc_11bf9 = 0x1225,
klocret_11c03 = 0x1226,
kloc_11c04 = 0x1227,
kloc_11c08 = 0x1228,
ksub_11c0c = 0x1229,
kloc_11c14 = 0x122a,
klocret_11c28 = 0x122b,
k_copy_printable = 0x122c,
kloc_11c33 = 0x122d,
kloc_11c40 = 0x122e,
k_clean_11c43 = 0x122f,
kloc_11cb8 = 0x1230,
kloc_11d2d = 0x1231,
k_ems_init = 0x1232,
kloc_11e00 = 0x1233,
k_ems_release = 0x1234,
klocret_11e1d = 0x1235,
k_ems_realloc = 0x1236,
klocret_11e36 = 0x1237,
k_ems_deinit = 0x1238,
klocret_11e46 = 0x1239,
k_ems_save_mapctx = 0x123a,
kloc_11e51 = 0x123b,
klocret_11e67 = 0x123c,
k_ems_restore_mapctx = 0x123d,
kloc_11e79 = 0x123e,
klocret_11e8a = 0x123f,
k_ems_mapmem = 0x1240,
kloc_11e9e = 0x1241,
kloc_11eb3 = 0x1242,
kloc_11eb6 = 0x1243,
klocret_11ec4 = 0x1244,
k_ems_mapmem2 = 0x1245,
kloc_11ed8 = 0x1246,
kloc_11eed = 0x1247,
kloc_11ef0 = 0x1248,
klocret_11efe = 0x1249,
k_ems_realloc2 = 0x124a,
kloc_11f3c = 0x124b,
k_mod_readfile_11f4e = 0x124c,
kloc_11f70 = 0x124d,
kloc_11fa9 = 0x124e,
kloc_11fcb = 0x124f,
kloc_11fd2 = 0x1250,
klocret_11fd3 = 0x1251,
kloc_11fd4 = 0x1252,
kloc_11fd6 = 0x1253,
kloc_11ff7 = 0x1254,
kloc_12027 = 0x1255,
kloc_12056 = 0x1256,
kloc_12066 = 0x1257,
kloc_1206b = 0x1258,
kloc_12071 = 0x1259,
kloc_12075 = 0x125a,
kloc_120aa = 0x125b,
kloc_120c4 = 0x125c,
kloc_120e7 = 0x125d,
kloc_120fd = 0x125e,
kloc_12106 = 0x125f,
kloc_12117 = 0x1260,
kloc_12123 = 0x1261,
kloc_1212b = 0x1262,
kloc_1213c = 0x1263,
kloc_1219e = 0x1264,
kloc_121b9 = 0x1265,
kloc_121cd = 0x1266,
kloc_121ee = 0x1267,
kloc_12215 = 0x1268,
kloc_12216 = 0x1269,
klocret_1221f = 0x126a,
k_mod_sub_12220 = 0x126b,
kloc_12228 = 0x126c,
kloc_12239 = 0x126d,
k_mod_readfile_12247 = 0x126e,
kloc_1224f = 0x126f,
kloc_12262 = 0x1270,
kloc_12271 = 0x1271,
kloc_122ae = 0x1272,
kloc_122b8 = 0x1273,
kloc_122da = 0x1274,
kloc_122dc = 0x1275,
kloc_122e3 = 0x1276,
klocret_122e7 = 0x1277,
k_ems_mapmemx = 0x1278,
kloc_1234e = 0x1279,
kloc_1236c = 0x127a,
kloc_12386 = 0x127b,
kloc_123b0 = 0x127c,
kloc_123ee = 0x127d,
kloc_1242d = 0x127e,
kloc_12466 = 0x127f,
kloc_1248b = 0x1280,
kloc_12493 = 0x1281,
kloc_12497 = 0x1282,
k_ems_mapmemy = 0x1283,
kloc_12508 = 0x1284,
kloc_12529 = 0x1285,
kloc_1253b = 0x1286,
kloc_12568 = 0x1287,
kloc_125a1 = 0x1288,
k_deinit_125b9 = 0x1289,
k_memfree_125da = 0x128a,
kloc_125f6 = 0x128b,
kloc_12612 = 0x128c,
kloc_12636 = 0x128d,
kloc_1263d = 0x128e,
kloc_12644 = 0x128f,
kloc_12655 = 0x1290,
kloc_1265b = 0x1291,
krender_1265d = 0x1292,
ksub_126a9 = 0x1293,
k_volume_prep = 0x1294,
kloc_126f0 = 0x1295,
kloc_12702 = 0x1296,
kloc_12721 = 0x1297,
kloc_1272d = 0x1298,
kloc_1275f = 0x1299,
kloc_1276c = 0x129a,
kloc_12780 = 0x129b,
kloc_1278f = 0x129c,
ksub_1279a = 0x129d,
kloc_127bd = 0x129e,
kloc_127ce = 0x129f,
kloc_127fc = 0x12a0,
kloc_1280d = 0x12a1,
ksub_1281a = 0x12a2,
k_update_sound_cyclic_buffer = 0x12a3,
kloc_12870 = 0x12a4,
kloc_12898 = 0x12a5,
kloc_128bb = 0x12a6,
kloc_128dd = 0x12a7,
kloc_12913 = 0x12a8,
kloc_1291a = 0x12a9,
kloc_1291e = 0x12aa,
kloc_12921 = 0x12ab,
klocret_12a55 = 0x12ac,
k_memclean = 0x12ad,
k_volume_12a66 = 0x12ae,
kloc_12a73 = 0x12af,
k_getset_volume = 0x12b0,
kloc_12a98 = 0x12b1,
kloc_12aa9 = 0x12b2,
k_getset_amplif = 0x12b3,
kloc_12acb = 0x12b4,
kloc_12ace = 0x12b5,
k_get_playsettings = 0x12b6,
k_set_playsettings = 0x12b7,
kloc_12afb = 0x12b8,
ksub_12afd = 0x12b9,
kloc_12b16 = 0x12ba,
ksub_12b18 = 0x12bb,
kloc_12b42 = 0x12bc,
kloc_12b5a = 0x12bd,
kloc_12b5f = 0x12be,
kloc_12b62 = 0x12bf,
kloc_12b71 = 0x12c0,
kloc_12b75 = 0x12c1,
ksub_12b83 = 0x12c2,
kloc_12b92 = 0x12c3,
kloc_12b98 = 0x12c4,
kloc_12ba6 = 0x12c5,
kloc_12bb3 = 0x12c6,
kloc_12bc0 = 0x12c7,
kloc_12bcb = 0x12c8,
kloc_12bef = 0x12c9,
k_someplaymode = 0x12ca,
kloc_12c3c = 0x12cb,
kloc_12c75 = 0x12cc,
kloc_12c86 = 0x12cd,
kloc_12c8f = 0x12ce,
k_getset_playstate = 0x12cf,
kloc_12ca7 = 0x12d0,
ksub_12cad = 0x12d1,
k_read_sndsettings = 0x12d2,
kloc_12cff = 0x12d3,
ksub_12d05 = 0x12d4,
kloc_12d2e = 0x12d5,
kloc_12d30 = 0x12d6,
ksub_12d35 = 0x12d7,
kloc_12d41 = 0x12d8,
kloc_12d4e = 0x12d9,
ksub_12da8 = 0x12da,
kloc_12e55 = 0x12db,
kloc_12e6b = 0x12dc,
kloc_12e74 = 0x12dd,
kloc_12e7d = 0x12de,
kloc_12e9f = 0x12df,
kloc_12eae = 0x12e0,
kloc_12eb2 = 0x12e1,
ksub_12eba = 0x12e2,
k_snd_offx = 0x12e3,
ksub_12f56 = 0x12e4,
kloc_12f78 = 0x12e5,
k_get_12f7c = 0x12e6,
k_set_timer_int = 0x12e7,
klocret_12fb3 = 0x12e8,
k_clean_int8_mem_timr = 0x12e9,
k_configure_timer = 0x12ea,
k_memfill8080 = 0x12eb,
ksub_13017 = 0x12ec,
kloc_1301e = 0x12ed,
kloc_1302c = 0x12ee,
kloc_13038 = 0x12ef,
ksub_13044 = 0x12f0,
kloc_1305a = 0x12f1,
kloc_1306d = 0x12f2,
kloc_13080 = 0x12f3,
kloc_13091 = 0x12f4,
kloc_130a2 = 0x12f5,
kloc_130ae = 0x12f6,
kloc_130bc = 0x12f7,
kloc_130f6 = 0x12f8,
kloc_1310d = 0x12f9,
kloc_13120 = 0x12fa,
kloc_13127 = 0x12fb,
kloc_13131 = 0x12fc,
kloc_13148 = 0x12fd,
kloc_1315a = 0x12fe,
kloc_13162 = 0x12ff,
kloc_1316b = 0x1300,
kloc_13171 = 0x1301,
ksub_13177 = 0x1302,
klocret_131b2 = 0x1303,
kloc_131b3 = 0x1304,
k_nullsub_5 = 0x1305,
kloc_131d0 = 0x1306,
ksub_131da = 0x1307,
klocret_131ee = 0x1308,
ksub_131ef = 0x1309,
kloc_13202 = 0x130a,
ksub_13429 = 0x130b,
kloc_13471 = 0x130c,
klocret_13498 = 0x130d,
kloc_13499 = 0x130e,
ksub_3_135ca = 0x130f,
kloc_135d3 = 0x1310,
kloc_135e0 = 0x1311,
kloc_135f2 = 0x1312,
kloc_135fd = 0x1313,
kloc_13608 = 0x1314,
kloc_1361c = 0x1315,
ksub_13623 = 0x1316,
kloc_13646 = 0x1317,
kloc_13661 = 0x1318,
kloc_13677 = 0x1319,
kloc_136cb = 0x131a,
kloc_13705 = 0x131b,
kloc_13718 = 0x131c,
kloc_13742 = 0x131d,
kloc_13791 = 0x131e,
kloc_1379f = 0x131f,
kloc_137a9 = 0x1320,
kloc_137be = 0x1321,
kloc_137ce = 0x1322,
ksub_137d5 = 0x1323,
kloc_137f0 = 0x1324,
klocret_13812 = 0x1325,
kchanl_2_eff_13813 = 0x1326,
ksub_13826 = 0x1327,
kloc_13848 = 0x1328,
kloc_13863 = 0x1329,
kloc_1386c = 0x132a,
klocret_1387d = 0x132b,
k_eff_nullsub = 0x132c,
k_eff_1387f = 0x132d,
k_eff_13886 = 0x132e,
kloc_1388b = 0x132f,
kloc_13897 = 0x1330,
k_eff_1389d = 0x1331,
k_eff_138a4 = 0x1332,
kloc_138a9 = 0x1333,
kloc_138b3 = 0x1334,
kloc_138b7 = 0x1335,
kloc_138bd = 0x1336,
kloc_138c7 = 0x1337,
k_eff_138d2 = 0x1338,
kloc_138de = 0x1339,
kloc_138f6 = 0x133a,
kloc_138fc = 0x133b,
kloc_1390b = 0x133c,
kloc_13917 = 0x133d,
kloc_1391f = 0x133e,
kloc_13929 = 0x133f,
k_eff_1392f = 0x1340,
kloc_13931 = 0x1341,
kloc_13943 = 0x1342,
kloc_1394d = 0x1343,
kloc_13950 = 0x1344,
kloc_1396d = 0x1345,
kloc_1397b = 0x1346,
kloc_13981 = 0x1347,
kloc_1399d = 0x1348,
k_eff_139ac = 0x1349,
k_eff_139b2 = 0x134a,
k_eff_139b9 = 0x134b,
kloc_139cb = 0x134c,
kloc_139d5 = 0x134d,
kloc_139d8 = 0x134e,
kloc_139f8 = 0x134f,
kloc_13a06 = 0x1350,
kloc_13a0c = 0x1351,
kloc_13a30 = 0x1352,
kloc_13a36 = 0x1353,
k_eff_13a43 = 0x1354,
klocret_13a5a = 0x1355,
kloc_13a5b = 0x1356,
kloc_13a60 = 0x1357,
kloc_13a65 = 0x1358,
k_eff_13a94 = 0x1359,
kloc_13a9b = 0x135a,
kloc_13aae = 0x135b,
kloc_13ac6 = 0x135c,
kloc_13ace = 0x135d,
k_eff_13ad7 = 0x135e,
kloc_13ade = 0x135f,
kloc_13ae0 = 0x1360,
kloc_13ae8 = 0x1361,
kloc_13aef = 0x1362,
kloc_13af2 = 0x1363,
kloc_13aff = 0x1364,
k_eff_13b06 = 0x1365,
kloc_13b29 = 0x1366,
kloc_13b34 = 0x1367,
kloc_13b3e = 0x1368,
kloc_13b48 = 0x1369,
kloc_13b50 = 0x136a,
kloc_13b5b = 0x136b,
kloc_13b66 = 0x136c,
kloc_13b72 = 0x136d,
k_eff_13b78 = 0x136e,
kloc_13b81 = 0x136f,
k_eff_13b88 = 0x1370,
k_eff_13ba3 = 0x1371,
k_eff_13bb2 = 0x1372,
kloc_13bbb = 0x1373,
k_eff_13bc0 = 0x1374,
k_eff_13bc8 = 0x1375,
kloc_13be7 = 0x1376,
kloc_13bf1 = 0x1377,
k_eff_13c02 = 0x1378,
kloc_13c1a = 0x1379,
kloc_13c2d = 0x137a,
k_eff_13c34 = 0x137b,
k_eff_13c3f = 0x137c,
kloc_13c47 = 0x137d,
k_eff_13c64 = 0x137e,
kloc_13c77 = 0x137f,
k_eff_13c88 = 0x1380,
k_eff_13c95 = 0x1381,
k_eff_13ca2 = 0x1382,
kloc_13cae = 0x1383,
k_eff_13cb3 = 0x1384,
k_eff_13cc9 = 0x1385,
k_eff_13cdd = 0x1386,
k_eff_13ce8 = 0x1387,
klocret_13cf4 = 0x1388,
klocret_13cf5 = 0x1389,
ksub_13cf6 = 0x138a,
kloc_13d36 = 0x138b,
kloc_13d4b = 0x138c,
kloc_13d8d = 0x138d,
ksub_13d95 = 0x138e,
kloc_13d9a = 0x138f,
k_settimer = 0x1390,
k_set_timer = 0x1391,
k_clean_timer = 0x1392,
k_eff_13de5 = 0x1393,
k_eff_13def = 0x1394,
kloc_13df9 = 0x1395,
kloc_13e03 = 0x1396,
kloc_13e18 = 0x1397,
k_eff_13e1e = 0x1398,
kloc_13e2a = 0x1399,
k_eff_13e2d = 0x139a,
k_eff_13e32 = 0x139b,
kloc_13e39 = 0x139c,
kloc_13e5e = 0x139d,
kloc_13e6f = 0x139e,
k_eff_13e7f = 0x139f,
k_eff_13e84 = 0x13a0,
k_eff_13e8c = 0x13a1,
ksub_13e9b = 0x13a2,
kloc_13ed3 = 0x13a3,
kloc_13ee8 = 0x13a4,
k_nullsub_2 = 0x13a5,
k_eff_13f05 = 0x13a6,
kloc_13f0c = 0x13a7,
kloc_13f34 = 0x13a8,
k_eff_13f3b = 0x13a9,
kloc_13f42 = 0x13aa,
kloc_13f6d = 0x13ab,
kloc_13f7c = 0x13ac,
kloc_13f81 = 0x13ad,
kloc_13f8f = 0x13ae,
kloc_13f96 = 0x13af,
kloc_13fab = 0x13b0,
kloc_13fb4 = 0x13b1,
kloc_13fb7 = 0x13b2,
k_eff_13fbe = 0x13b3,
kloc_13fce = 0x13b4,
kloc_14000 = 0x13b5,
kloc_1401a = 0x13b6,
k_eff_14020 = 0x13b7,
k_eff_14030 = 0x13b8,
kloc_1403d = 0x13b9,
k_calc_14043 = 0x13ba,
k_eff_14067 = 0x13bb,
kloc_14077 = 0x13bc,
kloc_14080 = 0x13bd,
ksub_14087 = 0x13be,
kloc_14090 = 0x13bf,
kloc_140a2 = 0x13c0,
klocret_140b2 = 0x13c1,
kloc_140b3 = 0x13c2,
kprepare_channels_1_140b6 = 0x13c3,
kloc_140d8 = 0x13c4,
klocret_140e5 = 0x13c5,
kloc_140e6 = 0x13c6,
kloc_140f7 = 0x13c7,
kloc_140fe = 0x13c8,
kloc_14111 = 0x13c9,
kloc_14131 = 0x13ca,
kloc_1413e = 0x13cb,
kloc_14142 = 0x13cc,
kloc_14153 = 0x13cd,
ksub_1415e = 0x13ce,
kloc_14184 = 0x13cf,
kloc_1419e = 0x13d0,
kloc_141a2 = 0x13d1,
kloc_141ba = 0x13d2,
kloc_141da = 0x13d3,
k_vlm_141df = 0x13d4,
k_snd_initialze = 0x13d5,
kloc_1420d = 0x13d6,
k_snd_on = 0x13d7,
k_snd_off = 0x13d8,
k_snd_deinit = 0x13d9,
kloc_14332 = 0x13da,
k_wss_int = 0x13db,
k_sb16_init = 0x13dc,
kloc_14ab3 = 0x13dd,
kloc_14abb = 0x13de,
kloc_14ae7 = 0x13df,
kloc_14af5 = 0x13e0,
kloc_14afd = 0x13e1,
k_sb16_on = 0x13e2,
kloc_14b36 = 0x13e3,
kloc_14b3e = 0x13e4,
kloc_14b47 = 0x13e5,
kloc_14b50 = 0x13e6,
kloc_14b6a = 0x13e7,
kloc_14b76 = 0x13e8,
kloc_14b77 = 0x13e9,
kloc_14b87 = 0x13ea,
kloc_14ba0 = 0x13eb,
k_sb_callback = 0x13ec,
k_sb16_off = 0x13ed,
kloc_14bd8 = 0x13ee,
kloc_14be8 = 0x13ef,
kloc_14be9 = 0x13f0,
kloc_14bfd = 0x13f1,
k_sb16_deinit = 0x13f2,
k_sbpro_set = 0x13f3,
kloc_14c89 = 0x13f4,
kloc_14c99 = 0x13f5,
kloc_14ca1 = 0x13f6,
kloc_14caa = 0x13f7,
kloc_14cb2 = 0x13f8,
kloc_14cbf = 0x13f9,
kloc_14cc7 = 0x13fa,
kloc_14ce8 = 0x13fb,
kloc_14e10 = 0x13fc,
kloc_14e29 = 0x13fd,
kloc_14e4d = 0x13fe,
kloc_14e66 = 0x13ff,
kloc_14e6e = 0x1400,
kloc_14e79 = 0x1401,
kloc_14e8c = 0x1402,
kloc_14ea1 = 0x1403,
k_lc_disable_interpol = 0x1404,
kloc_14ecc = 0x1405,
k_timer_int_end = 0x1406,
kloc_14f3c = 0x1407,
kloc_14f50 = 0x1408,
k_covox_init = 0x1409,
kloc_14f95 = 0x140a,
k_covox_on = 0x140b,
k_covox_timer_int = 0x140c,
kloc_14fe3 = 0x140d,
k_covox_off = 0x140e,
k_covox_deinit = 0x140f,
k_stereo_init = 0x1410,
kloc_1501d = 0x1411,
k_stereo_set = 0x1412,
k_stereo_timer_int = 0x1413,
kloc_15047 = 0x1414,
kloc_1507e = 0x1415,
k_stereo_sndoff = 0x1416,
k_stereo_clean = 0x1417,
k_adlib_init = 0x1418,
kloc_150e8 = 0x1419,
k_adlib_set = 0x141a,
k_adlib_timer_int = 0x141b,
kloc_15120 = 0x141c,
kloc_1513c = 0x141d,
kloc_1514e = 0x141e,
k_adlib_sndoff = 0x141f,
k_adlib_clean = 0x1420,
k_pcspeaker_init = 0x1421,
k_pcspeaker_on = 0x1422,
k_pcspeaker_interrupt = 0x1423,
kloc_151c9 = 0x1424,
k_pcspeaker_off = 0x1425,
k_pcspeaker_deinit = 0x1426,
knn = 0x1427,
k_midi_init = 0x1428,
kloc_15302 = 0x1429,
kloc_15325 = 0x142a,
k_midi_set = 0x142b,
k_midi_int8p = 0x142c,
kloc_15380 = 0x142d,
kloc_1538f = 0x142e,
kloc_1539a = 0x142f,
k_midi_sndoff = 0x1430,
k_midi_clean = 0x1431,
k_midi_153c0 = 0x1432,
k_midi_153d6 = 0x1433,
kloc_153d8 = 0x1434,
k_midi_153f1 = 0x1435,
kloc_153f8 = 0x1436,
kloc_15401 = 0x1437,
kloc_15406 = 0x1438,
kloc_1540e = 0x1439,
k_midi_15413 = 0x143a,
kloc_15421 = 0x143b,
kloc_15428 = 0x143c,
kloc_15434 = 0x143d,
kloc_15439 = 0x143e,
klocret_15441 = 0x143f,
k_midi_15442 = 0x1440,
kloc_15447 = 0x1441,
k_nullsub_4 = 0x1442,
k_midi_1544d = 0x1443,
k_midi_15466 = 0x1444,
kloc_1546f = 0x1445,
kloc_1548d = 0x1446,
k_midi_154ac = 0x1447,
kloc_154b5 = 0x1448,
klocret_154d9 = 0x1449,
k_midi_154da = 0x144a,
k_midi_154de = 0x144b,
ksub_154f4 = 0x144c,
kloc_15525 = 0x144d,
k_lc_inerpol_disabld = 0x144e,
kchanel_15577 = 0x144f,
kloc_155a8 = 0x1450,
kloc_155b8 = 0x1451,
kloc_155c8 = 0x1452,
kloc_155d8 = 0x1453,
kloc_155e8 = 0x1454,
kloc_155f8 = 0x1455,
kloc_15608 = 0x1456,
kloc_15618 = 0x1457,
kloc_15621 = 0x1458,
kloc_15628 = 0x1459,
kloc_15638 = 0x145a,
kloc_15648 = 0x145b,
kloc_15658 = 0x145c,
kloc_15668 = 0x145d,
kloc_15678 = 0x145e,
kloc_15688 = 0x145f,
kloc_15698 = 0x1460,
kloc_156a1 = 0x1461,
kloc_1578c = 0x1462,
klocret_157bc = 0x1463,
kloc_157bd = 0x1464,
kloc_157d3 = 0x1465,
kloc_157dd = 0x1466,
kloc_157e5 = 0x1467,
k_lc_perfrm_interpol = 0x1468,
kloc_15877 = 0x1469,
kloc_15891 = 0x146a,
kloc_158c0 = 0x146b,
kloc_158ef = 0x146c,
kloc_1591e = 0x146d,
kloc_1594d = 0x146e,
kloc_1597c = 0x146f,
kloc_159ab = 0x1470,
kloc_159da = 0x1471,
kloc_15a09 = 0x1472,
kloc_15a38 = 0x1473,
kloc_15a67 = 0x1474,
kloc_15a96 = 0x1475,
kloc_15ac5 = 0x1476,
kloc_15af4 = 0x1477,
kloc_15b23 = 0x1478,
kloc_15b52 = 0x1479,
kloc_15b5b = 0x147a,
kloc_15e3d = 0x147b,
kloc_15e48 = 0x147c,
kloc_15e5b = 0x147d,
kloc_15e6e = 0x147e,
kloc_15e81 = 0x147f,
kloc_15e94 = 0x1480,
kloc_15ea7 = 0x1481,
kloc_15eba = 0x1482,
kloc_15ecd = 0x1483,
kloc_15ee0 = 0x1484,
kloc_15ef3 = 0x1485,
kloc_15f06 = 0x1486,
kloc_15f19 = 0x1487,
kloc_15f2c = 0x1488,
kloc_15f3f = 0x1489,
kloc_15f52 = 0x148a,
kloc_15f65 = 0x148b,
kloc_15f78 = 0x148c,
kloc_15f81 = 0x148d,
kchanel_1609f = 0x148e,
kloc_160d0 = 0x148f,
kloc_160e0 = 0x1490,
kloc_160f0 = 0x1491,
kloc_16100 = 0x1492,
kloc_16110 = 0x1493,
kloc_16120 = 0x1494,
kloc_16130 = 0x1495,
kloc_16140 = 0x1496,
kloc_16150 = 0x1497,
kloc_16160 = 0x1498,
kloc_16165 = 0x1499,
kloc_16170 = 0x149a,
kloc_16180 = 0x149b,
kloc_16190 = 0x149c,
kloc_161a0 = 0x149d,
kloc_161b0 = 0x149e,
kloc_161c0 = 0x149f,
kloc_161c9 = 0x14a0,
k_lc_perfrm_interpol2 = 0x14a1,
kloc_1633c = 0x14a2,
kloc_16356 = 0x14a3,
kloc_16369 = 0x14a4,
kloc_16385 = 0x14a5,
kloc_163b4 = 0x14a6,
kloc_163e3 = 0x14a7,
kloc_16412 = 0x14a8,
kloc_16441 = 0x14a9,
kloc_1646d = 0x14aa,
kloc_16470 = 0x14ab,
kloc_1649f = 0x14ac,
kloc_164ce = 0x14ad,
kloc_164fd = 0x14ae,
kloc_1652c = 0x14af,
kloc_1655b = 0x14b0,
kloc_16568 = 0x14b1,
kloc_1658a = 0x14b2,
kloc_165b9 = 0x14b3,
kloc_165e8 = 0x14b4,
kloc_16617 = 0x14b5,
kloc_16620 = 0x14b6,
kloc_16689 = 0x14b7,
kloc_1676a = 0x14b8,
kloc_16900 = 0x14b9,
kloc_1690b = 0x14ba,
kloc_16929 = 0x14bb,
kloc_16942 = 0x14bc,
kloc_16959 = 0x14bd,
kloc_16963 = 0x14be,
kloc_1696c = 0x14bf,
kloc_1697f = 0x14c0,
kloc_16992 = 0x14c1,
kloc_169a5 = 0x14c2,
kloc_169b8 = 0x14c3,
kloc_169cb = 0x14c4,
kloc_169de = 0x14c5,
kloc_169f1 = 0x14c6,
kloc_16a04 = 0x14c7,
kloc_16a17 = 0x14c8,
kloc_16a2a = 0x14c9,
kloc_16a3d = 0x14ca,
kloc_16a50 = 0x14cb,
kloc_16a63 = 0x14cc,
kloc_16a76 = 0x14cd,
kloc_16a89 = 0x14ce,
kloc_16a92 = 0x14cf,
kloc_16bb0 = 0x14d0,
kloc_16bc6 = 0x14d1,
kloc_16bcc = 0x14d2,
kloc_16bd2 = 0x14d3,
kloc_16bd8 = 0x14d4,
kloc_16bde = 0x14d5,
kloc_16be4 = 0x14d6,
kloc_16bea = 0x14d7,
kloc_16bf0 = 0x14d8,
kloc_16bf6 = 0x14d9,
kloc_16bfc = 0x14da,
kloc_16c02 = 0x14db,
kloc_16c08 = 0x14dc,
kloc_16c0e = 0x14dd,
kloc_16c14 = 0x14de,
kloc_16c1a = 0x14df,
kloc_16c20 = 0x14e0,
kloc_16c22 = 0x14e1,
kloc_16c66 = 0x14e2,
klocret_16c68 = 0x14e3,
kprepare_samples = 0x14e4,
kloc_16c88 = 0x14e5,
kloc_16c9d = 0x14e6,
kloc_16cb9 = 0x14e7,
kloc_16cbc = 0x14e8,
kloc_16cbe = 0x14e9,
kloc_16ceb = 0x14ea,
kloc_16cee = 0x14eb,
kchanel_16cf6 = 0x14ec,
kloc_16d0b = 0x14ed,
kloc_16d16 = 0x14ee,
kloc_16d21 = 0x14ef,
kloc_16d2c = 0x14f0,
kloc_16d37 = 0x14f1,
kloc_16d42 = 0x14f2,
kloc_16d4d = 0x14f3,
kloc_16d58 = 0x14f4,
kloc_16d5f = 0x14f5,
kloc_16d63 = 0x14f6,
kloc_16d6e = 0x14f7,
kloc_16d79 = 0x14f8,
kloc_16d84 = 0x14f9,
kloc_16d8f = 0x14fa,
kloc_16d9a = 0x14fb,
kloc_16da5 = 0x14fc,
kloc_16db0 = 0x14fd,
kloc_16dbb = 0x14fe,
klocret_16e23 = 0x14ff,
kloc_16e24 = 0x1500,
kloc_16e3f = 0x1501,
kloc_16e56 = 0x1502,
kloc_16e5d = 0x1503,
kloc_16e74 = 0x1504,
kloc_16e7b = 0x1505,
kloc_16e92 = 0x1506,
kloc_16e99 = 0x1507,
kloc_16eb0 = 0x1508,
kloc_16eb7 = 0x1509,
kloc_16ece = 0x150a,
kloc_16ed5 = 0x150b,
kloc_16eec = 0x150c,
kloc_16ef3 = 0x150d,
kloc_16f0a = 0x150e,
kloc_16f11 = 0x150f,
kloc_16f1d = 0x1510,
kloc_16f28 = 0x1511,
kloc_16f2f = 0x1512,
kloc_16f46 = 0x1513,
kloc_16f4d = 0x1514,
kloc_16f64 = 0x1515,
kloc_16f6b = 0x1516,
kloc_16f82 = 0x1517,
kloc_16f89 = 0x1518,
kloc_16fa0 = 0x1519,
kloc_16fa7 = 0x151a,
kloc_16fbe = 0x151b,
kloc_16fc5 = 0x151c,
kloc_16fdc = 0x151d,
kloc_16fe3 = 0x151e,
kloc_16ffa = 0x151f,
kloc_17001 = 0x1520,
kloc_17008 = 0x1521,
kloc_1701c = 0x1522,
kloc_17037 = 0x1523,
kloc_17053 = 0x1524,
kloc_1706f = 0x1525,
kloc_1708b = 0x1526,
kloc_170a7 = 0x1527,
kloc_170c3 = 0x1528,
kloc_170df = 0x1529,
kloc_170fb = 0x152a,
kloc_17117 = 0x152b,
kloc_17133 = 0x152c,
kloc_1714f = 0x152d,
kloc_1716b = 0x152e,
kloc_17187 = 0x152f,
kloc_171a3 = 0x1530,
kloc_171bf = 0x1531,
klocret_171d2 = 0x1532,
kloc_171d3 = 0x1533,
kloc_171da = 0x1534,
kloc_171f8 = 0x1535,
kloc_17202 = 0x1536,
kloc_17212 = 0x1537,
kloc_1721a = 0x1538,
kloc_17254 = 0x1539,
kloc_17257 = 0x153a,
ksub_1725f = 0x153b,
kloc_1727f = 0x153c,
kloc_1728f = 0x153d,
kloc_1729f = 0x153e,
kloc_172af = 0x153f,
kloc_172bf = 0x1540,
kloc_172cf = 0x1541,
kloc_172df = 0x1542,
kloc_172ef = 0x1543,
kloc_172ff = 0x1544,
kloc_1730f = 0x1545,
kloc_1731f = 0x1546,
kloc_1732f = 0x1547,
kloc_1733f = 0x1548,
kloc_1734f = 0x1549,
kloc_1735f = 0x154a,
kloc_1736f = 0x154b,
kloc_17376 = 0x154c,
klocret_17440 = 0x154d,
kloc_17441 = 0x154e,
kloc_1745c = 0x154f,
kloc_17473 = 0x1550,
kloc_1747a = 0x1551,
kloc_17491 = 0x1552,
kloc_17498 = 0x1553,
kloc_174af = 0x1554,
kloc_174b6 = 0x1555,
kloc_174cd = 0x1556,
kloc_174d4 = 0x1557,
kloc_174eb = 0x1558,
kloc_174f2 = 0x1559,
kloc_17509 = 0x155a,
kloc_17510 = 0x155b,
kloc_17527 = 0x155c,
kloc_1752e = 0x155d,
kloc_17545 = 0x155e,
kloc_1754c = 0x155f,
kloc_1754f = 0x1560,
kloc_17563 = 0x1561,
kloc_1756a = 0x1562,
kloc_17581 = 0x1563,
kloc_17588 = 0x1564,
kloc_1759f = 0x1565,
kloc_175a6 = 0x1566,
kloc_175bd = 0x1567,
kloc_175c4 = 0x1568,
kloc_175db = 0x1569,
kloc_175e2 = 0x156a,
kloc_175f9 = 0x156b,
kloc_17600 = 0x156c,
kloc_17617 = 0x156d,
kloc_1761e = 0x156e,
kloc_17625 = 0x156f,
kloc_17639 = 0x1570,
kloc_17654 = 0x1571,
kloc_17670 = 0x1572,
kloc_1768c = 0x1573,
kloc_176a8 = 0x1574,
kloc_176c4 = 0x1575,
kloc_176e0 = 0x1576,
kloc_176fc = 0x1577,
kloc_17718 = 0x1578,
kloc_17734 = 0x1579,
kloc_17750 = 0x157a,
kloc_1776c = 0x157b,
kloc_17788 = 0x157c,
kloc_177a4 = 0x157d,
kloc_177c0 = 0x157e,
kloc_177dc = 0x157f,
klocret_177ee = 0x1580,
k_lc_16bit = 0x1581,
kloc_17819 = 0x1582,
kloc_1781c = 0x1583,
ksub_17824 = 0x1584,
kloc_17839 = 0x1585,
kloc_1784c = 0x1586,
kloc_1785f = 0x1587,
kloc_17872 = 0x1588,
kloc_17885 = 0x1589,
kloc_17898 = 0x158a,
kloc_178ab = 0x158b,
kloc_178be = 0x158c,
kloc_178d1 = 0x158d,
kloc_178e4 = 0x158e,
kloc_178f7 = 0x158f,
kloc_1790a = 0x1590,
kloc_1791d = 0x1591,
kloc_17930 = 0x1592,
kloc_17943 = 0x1593,
kloc_17956 = 0x1594,
kloc_1795d = 0x1595,
klocret_17a57 = 0x1596,
kloc_17a58 = 0x1597,
kloc_17a72 = 0x1598,
kloc_17a89 = 0x1599,
kloc_17a8f = 0x159a,
kloc_17aa6 = 0x159b,
kloc_17aac = 0x159c,
kloc_17ac3 = 0x159d,
kloc_17ac9 = 0x159e,
kloc_17ae0 = 0x159f,
kloc_17ae6 = 0x15a0,
kloc_17afd = 0x15a1,
kloc_17b03 = 0x15a2,
kloc_17b1a = 0x15a3,
kloc_17b20 = 0x15a4,
kloc_17b37 = 0x15a5,
kloc_17b3d = 0x15a6,
kloc_17b54 = 0x15a7,
kloc_17b5a = 0x15a8,
kloc_17b71 = 0x15a9,
kloc_17b77 = 0x15aa,
kloc_17b8e = 0x15ab,
kloc_17b94 = 0x15ac,
kloc_17bab = 0x15ad,
kloc_17bb1 = 0x15ae,
kloc_17bc8 = 0x15af,
kloc_17bce = 0x15b0,
kloc_17be5 = 0x15b1,
kloc_17beb = 0x15b2,
kloc_17c02 = 0x15b3,
kloc_17c08 = 0x15b4,
kloc_17c1f = 0x15b5,
kloc_17c25 = 0x15b6,
kloc_17c2c = 0x15b7,
kloc_17c40 = 0x15b8,
kloc_17c58 = 0x15b9,
kloc_17c71 = 0x15ba,
kloc_17c8a = 0x15bb,
kloc_17ca3 = 0x15bc,
kloc_17cbc = 0x15bd,
kloc_17cd5 = 0x15be,
kloc_17cee = 0x15bf,
kloc_17d07 = 0x15c0,
kloc_17d20 = 0x15c1,
kloc_17d39 = 0x15c2,
kloc_17d52 = 0x15c3,
kloc_17d6b = 0x15c4,
kloc_17d84 = 0x15c5,
kloc_17d9d = 0x15c6,
kloc_17db6 = 0x15c7,
k_nullsub_3 = 0x15c8,
kloc_18077 = 0x15c9,
ksub_182db = 0x15ca,
k_nongravis_182e7 = 0x15cb,
kloc_182f7 = 0x15cc,
kloc_18338 = 0x15cd,
kloc_18360 = 0x15ce,
k_adlib_18389 = 0x15cf,
kloc_1838b = 0x15d0,
k_adlib_18395 = 0x15d1,
k_sb16_detect_port = 0x15d2,
kloc_183de = 0x15d3,
kloc_1842d = 0x15d4,
kloc_184c3 = 0x15d5,
kloc_184dc = 0x15d6,
kloc_18501 = 0x15d7,
k_sb16_sound_on = 0x15d8,
k_sb16_18540 = 0x15d9,
kloc_18591 = 0x15da,
kloc_185b5 = 0x15db,
kloc_185b8 = 0x15dc,
kloc_185cd = 0x15dd,
k_sb16_handler_int = 0x15de,
kloc_18616 = 0x15df,
kloc_1861f = 0x15e0,
kloc_18628 = 0x15e1,
kloc_18631 = 0x15e2,
kloc_1864d = 0x15e3,
kloc_18656 = 0x15e4,
kloc_1865f = 0x15e5,
kloc_18689 = 0x15e6,
kloc_18692 = 0x15e7,
kloc_1869b = 0x15e8,
kloc_186a4 = 0x15e9,
kloc_186c2 = 0x15ea,
kloc_186cd = 0x15eb,
kloc_186d8 = 0x15ec,
k_dma_186e3 = 0x15ed,
kloc_186ef = 0x15ee,
kloc_18761 = 0x15ef,
kloc_187a6 = 0x15f0,
kloc_187eb = 0x15f1,
kloc_18830 = 0x15f2,
kloc_18878 = 0x15f3,
kloc_188c2 = 0x15f4,
kloc_1890c = 0x15f5,
k_set_dmachn_mask = 0x15f6,
kloc_18961 = 0x15f7,
k_alloc_dma_buf = 0x15f8,
kloc_189db = 0x15f9,
kloc_18a0a = 0x15fa,
kloc_18a22 = 0x15fb,
k_memfree_18a28 = 0x15fc,
kloc_18a3b = 0x15fd,
k_setsnd_handler = 0x15fe,
kloc_18a5c = 0x15ff,
k_restore_intvector = 0x1600,
k_getint_vect = 0x1601,
k_setint_vect = 0x1602,
k_memalloc = 0x1603,
kloc_18ad9 = 0x1604,
k_memfree = 0x1605,
k_memrealloc = 0x1606,
kloc_18aff = 0x1607,
k_setmemallocstrat = 0x1608,
k_getmemallocstrat = 0x1609,
k_setmemalloc1 = 0x160a,
k_setmemalloc2 = 0x160b,
k_writemixersb = 0x160c,
k_readmixersb = 0x160d,
k_writesb = 0x160e,
kloc_18b70 = 0x160f,
kloc_18b7c = 0x1610,
k_readsb = 0x1611,
kloc_18b8e = 0x1612,
kloc_18b9c = 0x1613,
k_checksb = 0x1614,
kloc_18bb1 = 0x1615,
kloc_18bbe = 0x1616,
k_sb16_sound_off = 0x1617,
k_initclockfromrtc = 0x1618,
k_u32tox = 0x1619,
k_u16tox = 0x161a,
k_u8tox = 0x161b,
k_u4tox = 0x161c,
kloc_18c3d = 0x161d,
k_my_i8toa10_0 = 0x161e,
k_my_i16toa10_0 = 0x161f,
k_my_i32toa10_0 = 0x1620,
k_my_u8toa_10 = 0x1621,
k_my_u16toa_10 = 0x1622,
k_my_u32toa10_0 = 0x1623,
k_my_i32toa10_1 = 0x1624,
k_my_u32toa_0 = 0x1625,
kloc_18c75 = 0x1626,
k_my_putdigit = 0x1627,
k_myasmsprintf = 0x1628,
kloc_18c9f = 0x1629,
kloc_18ca2 = 0x162a,
k_mysprintf_0_nop = 0x162b,
k_mysprintf_1_offstr = 0x162c,
k_mysprintf_2_off8str = 0x162d,
k_mysprintf_3_off16str = 0x162e,
kloc_18cd3 = 0x162f,
k_mysprintf_4_u8toa = 0x1630,
k_mysprintf_5_u16toa = 0x1631,
k_mysprintf_6_u32toa = 0x1632,
k_mysprintf_7_i8toa = 0x1633,
k_mysprintf_8_i16toa = 0x1634,
k_mysprintf_9_i32toa = 0x1635,
k_mysprintf_10_u8tox = 0x1636,
k_mysprintf_11_u16tox = 0x1637,
k_mysprintf_12_u32tox = 0x1638,
k_mystrlen_0 = 0x1639,
kloc_18d93 = 0x163a,
k_strcpy_count_0 = 0x163b,
kloc_18da1 = 0x163c,
kloc_18da6 = 0x163d,
kloc_18db0 = 0x163e,
kloc_18db8 = 0x163f,
kloc_19050 = 0x1640,
kloc_19053 = 0x1641,
kloc_19057 = 0x1642,
kloc_1906e = 0x1643,
kloc_1907c = 0x1644,
kloc_19084 = 0x1645,
kloc_19086 = 0x1646,
kloc_1908d = 0x1647,
kloc_19090 = 0x1648,
k_start = 0x1649,
kloc_19095 = 0x164a,
kloc_190a2 = 0x164b,
kloc_190b1 = 0x164c,
kloc_190bc = 0x164d,
kloc_190ce = 0x164e,
kloc_190d3 = 0x164f,
kloc_190e2 = 0x1650,
kloc_190f7 = 0x1651,
kloc_19103 = 0x1652,
kloc_19114 = 0x1653,
kloc_19125 = 0x1654,
kloc_19131 = 0x1655,
kloc_1913d = 0x1656,
kloc_19149 = 0x1657,
kloc_19155 = 0x1658,
kloc_19161 = 0x1659,
kloc_1917d = 0x165a,
kloc_191a2 = 0x165b,
kloc_191a5 = 0x165c,
kloc_191db = 0x165d,
kloc_191ea = 0x165e,
kloc_19212 = 0x165f,
kloc_19242 = 0x1660,
kloc_19250 = 0x1661,
kloc_19256 = 0x1662,
kloc_192b9 = 0x1663,
kloc_192c3 = 0x1664,
kloc_192ca = 0x1665,
kloc_192e0 = 0x1666,
kloc_192f7 = 0x1667,
kloc_192ff = 0x1668,
kloc_19395 = 0x1669,
kloc_193ae = 0x166a,
kloc_193bc = 0x166b,
kloc_193c7 = 0x166c,
kloc_193fc = 0x166d,
kloc_193ff = 0x166e,
kloc_19439 = 0x166f,
kloc_19445 = 0x1670,
kloc_19464 = 0x1671,
kloc_1949e = 0x1672,
kloc_194b9 = 0x1673,
kloc_194ce = 0x1674,
kloc_194da = 0x1675,
kloc_194e3 = 0x1676,
kloc_194eb = 0x1677,
kloc_19506 = 0x1678,
kloc_1953c = 0x1679,
kloc_1955d = 0x167a,
kloc_1957f = 0x167b,
kloc_19595 = 0x167c,
kloc_195a7 = 0x167d,
kloc_195be = 0x167e,
kloc_195de = 0x167f,
kloc_195ea = 0x1680,
kloc_19610 = 0x1681,
kloc_19648 = 0x1682,
kloc_1964e = 0x1683,
kloc_19657 = 0x1684,
kloc_19698 = 0x1685,
kloc_196b0 = 0x1686,
kloc_196d0 = 0x1687,
kloc_196f1 = 0x1688,
kloc_19744 = 0x1689,
kloc_19762 = 0x168a,
kloc_19788 = 0x168b,
kloc_197bf = 0x168c,
kloc_197d6 = 0x168d,
kloc_197e7 = 0x168e,
kloc_19827 = 0x168f,
kloc_19848 = 0x1690,
kloc_1987c = 0x1691,
kloc_19880 = 0x1692,
k_dosgetcurdir = 0x1693,
k_doschdir = 0x1694,
k_recolortxtx = 0x1695,
k_filelist_198b8 = 0x1696,
kloc_198d5 = 0x1697,
kloc_198e7 = 0x1698,
kloc_198fd = 0x1699,
kloc_19903 = 0x169a,
kloc_19914 = 0x169b,
kloc_19925 = 0x169c,
kloc_1992a = 0x169d,
kloc_1993d = 0x169e,
kloc_19950 = 0x169f,
kloc_19958 = 0x16a0,
kloc_199cf = 0x16a1,
kloc_199d4 = 0x16a2,
kloc_199e7 = 0x16a3,
k_recolortxt = 0x16a4,
kloc_19a04 = 0x16a5,
k_cpy_printable = 0x16a6,
kloc_19a17 = 0x16a7,
kloc_19a25 = 0x16a8,
kloc_19a34 = 0x16a9,
klocret_19a3e = 0x16aa,
kloc_19a46 = 0x16ab,
klocret_19a52 = 0x16ac,
k_modules_search = 0x16ad,
kloc_19a6e = 0x16ae,
kloc_19ac3 = 0x16af,
kloc_19b1d = 0x16b0,
kloc_19b24 = 0x16b1,
kloc_19b3c = 0x16b2,
kloc_19b6a = 0x16b3,
kloc_19b7d = 0x16b4,
kloc_19b83 = 0x16b5,
kloc_19bb4 = 0x16b6,
kloc_19bdd = 0x16b7,
kloc_19c71 = 0x16b8,
kloc_19c77 = 0x16b9,
kloc_19c80 = 0x16ba,
kloc_19c86 = 0x16bb,
kloc_19c99 = 0x16bc,
kloc_19ca2 = 0x16bd,
kloc_19ca9 = 0x16be,
kloc_19cdf = 0x16bf,
kloc_19ce1 = 0x16c0,
k_parse_cmdline = 0x16c1,
kloc_19d19 = 0x16c2,
kloc_19d2c = 0x16c3,
kloc_19d47 = 0x16c4,
kloc_19d4e = 0x16c5,
kloc_19d63 = 0x16c6,
kloc_19d64 = 0x16c7,
k_readallmoules = 0x16c8,
kloc_19d75 = 0x16c9,
kloc_19d81 = 0x16ca,
kloc_19d83 = 0x16cb,
kloc_19d94 = 0x16cc,
kloc_19db8 = 0x16cd,
kloc_19dbb = 0x16ce,
kloc_19df9 = 0x16cf,
kloc_19e03 = 0x16d0,
kloc_19e09 = 0x16d1,
k_read_module = 0x16d2,
kloc_19e22 = 0x16d3,
kloc_19e2c = 0x16d4,
kloc_19e41 = 0x16d5,
kloc_19e47 = 0x16d6,
kloc_19e58 = 0x16d7,
kloc_19e5e = 0x16d8,
kloc_19eba = 0x16d9,
kloc_19ec7 = 0x16da,
kloc_19ecc = 0x16db,
k_keyb_screen_loop = 0x16dc,
kloc_19f6c = 0x16dd,
kloc_1a042 = 0x16de,
k_l_1a044 = 0x16df,
kloc_1a070 = 0x16e0,
kloc_1a0a0 = 0x16e1,
k_l_up = 0x16e2,
k_l_down = 0x16e3,
k_l_right = 0x16e4,
kloc_1a0e6 = 0x16e5,
kloc_1a0f2 = 0x16e6,
k_l_left = 0x16e7,
kloc_1a118 = 0x16e8,
k_l_l = 0x16e9,
k_l_m = 0x16ea,
k_l_r = 0x16eb,
k_l_s = 0x16ec,
k_l_plus = 0x16ed,
kloc_1a14b = 0x16ee,
kloc_1a155 = 0x16ef,
k_l_minus = 0x16f0,
kloc_1a174 = 0x16f1,
kloc_1a17a = 0x16f2,
k_l_rbracket = 0x16f3,
kloc_1a199 = 0x16f4,
kloc_1a1a3 = 0x16f5,
k_l_lbracket = 0x16f6,
kloc_1a1c2 = 0x16f7,
kloc_1a1c9 = 0x16f8,
kloc_1a1d1 = 0x16f9,
k_l_f1 = 0x16fa,
k_l_f2 = 0x16fb,
k_l_f3 = 0x16fc,
k_l_f4 = 0x16fd,
kloc_1a219 = 0x16fe,
kloc_1a21f = 0x16ff,
k_l_f5 = 0x1700,
k_l_f6 = 0x1701,
k_l_f8 = 0x1702,
k_l_f9 = 0x1703,
k_l_f10 = 0x1704,
k_l_f11 = 0x1705,
k_l_f12 = 0x1706,
kloc_1a288 = 0x1707,
kloc_1a28f = 0x1708,
k_l_tab = 0x1709,
kloc_1a2a0 = 0x170a,
kloc_1a2be = 0x170b,
kloc_1a2c1 = 0x170c,
kloc_1a2d1 = 0x170d,
kloc_1a2e1 = 0x170e,
k_l_numlock = 0x170f,
kloc_1a30d = 0x1710,
k_l_scrollock = 0x1711,
kloc_1a326 = 0x1712,
k_l_1_end = 0x1713,
kloc_1a33e = 0x1714,
kloc_1a34b = 0x1715,
kloc_1a356 = 0x1716,
k_l_enter = 0x1717,
k_l_esc = 0x1718,
kloc_1a393 = 0x1719,
kloc_1a3a7 = 0x171a,
kloc_1a3c5 = 0x171b,
kloc_1a3f6 = 0x171c,
k_f1_help = 0x171d,
k_f2_waves = 0x171e,
k_f3_textmetter = 0x171f,
k_f4_patternnae = 0x1720,
k_f5_graphspectr = 0x1721,
k_f6_undoc = 0x1722,
k_text_init = 0x1723,
kloc_1a4a6 = 0x1724,
k_text_init2 = 0x1725,
k_setvideomode = 0x1726,
kloc_1a4d5 = 0x1727,
kloc_1a4e8 = 0x1728,
klocret_1a4f1 = 0x1729,
kloc_1a4f2 = 0x172a,
kloc_1a529 = 0x172b,
kloc_1a545 = 0x172c,
kloc_1a55b = 0x172d,
kloc_1a5ab = 0x172e,
kloc_1a5c3 = 0x172f,
kloc_1a5c5 = 0x1730,
kloc_1a61a = 0x1731,
kloc_1a628 = 0x1732,
kloc_1a645 = 0x1733,
kloc_1a687 = 0x1734,
kloc_1a691 = 0x1735,
kloc_1a699 = 0x1736,
kloc_1a6a1 = 0x1737,
kloc_1a6b7 = 0x1738,
kloc_1a6c2 = 0x1739,
kloc_1a74d = 0x173a,
k_txt_draw_top_title = 0x173b,
k_txt_draw_bottom = 0x173c,
kloc_1a7cc = 0x173d,
kloc_1a83e = 0x173e,
kloc_1a856 = 0x173f,
kloc_1a86e = 0x1740,
kloc_1a886 = 0x1741,
kloc_1a8eb = 0x1742,
k_f3_draw = 0x1743,
kloc_1a913 = 0x1744,
kloc_1a934 = 0x1745,
kloc_1a93a = 0x1746,
kloc_1a947 = 0x1747,
kloc_1a951 = 0x1748,
kloc_1a975 = 0x1749,
kloc_1a9a5 = 0x174a,
kloc_1a9a8 = 0x174b,
kput_sample_name = 0x174c,
kloc_1a9c2 = 0x174d,
kloc_1aa17 = 0x174e,
kloc_1aa1a = 0x174f,
kloc_1aa2e = 0x1750,
kloc_1aa36 = 0x1751,
kloc_1aa4f = 0x1752,
kloc_1aa5c = 0x1753,
kloc_1aa62 = 0x1754,
kloc_1aa73 = 0x1755,
kloc_1aa88 = 0x1756,
k_volume_medium = 0x1757,
k_volume_higher = 0x1758,
k_volume_endstr = 0x1759,
kloc_1aacb = 0x175a,
kloc_1aaf0 = 0x175b,
kloc_1aaf7 = 0x175c,
kloc_1ab0d = 0x175d,
kloc_1ab44 = 0x175e,
kloc_1ab53 = 0x175f,
kloc_1ab5d = 0x1760,
kloc_1ab67 = 0x1761,
kloc_1ab6f = 0x1762,
ksub_1ab8c = 0x1763,
kloc_1ab9b = 0x1764,
kloc_1abab = 0x1765,
k_txt_1abae = 0x1766,
kloc_1abb3 = 0x1767,
k_f4_draw = 0x1768,
kloc_1abf0 = 0x1769,
kloc_1ac35 = 0x176a,
kloc_1ac5f = 0x176b,
kloc_1ac6d = 0x176c,
kloc_1acac = 0x176d,
kloc_1acd2 = 0x176e,
klocret_1acf5 = 0x176f,
k_my_pnt_u32toa_fill = 0x1770,
k_my_u32toa_fill = 0x1771,
kloc_1ad0f = 0x1772,
k_f1_draw = 0x1773,
k_init_vga_waves = 0x1774,
kloc_1ade0 = 0x1775,
kloc_1ade2 = 0x1776,
kloc_1ade7 = 0x1777,
kloc_1adf0 = 0x1778,
kloc_1adf6 = 0x1779,
kloc_1ae05 = 0x177a,
kloc_1ae0c = 0x177b,
kloc_1ae11 = 0x177c,
kloc_1ae16 = 0x177d,
k_read2buffer = 0x177e,
k_init_vga_waves_chunk = 0x177f,
kloc_1ae2d = 0x1780,
kloc_1ae3a = 0x1781,
kloc_1ae46 = 0x1782,
kloc_1ae5e = 0x1783,
kloc_1ae66 = 0x1784,
kloc_1ae7e = 0x1785,
kloc_1aeb2 = 0x1786,
k_f2_draw_waves = 0x1787,
k_lc_next_meter = 0x1788,
k_lc_nextvideobit = 0x1789,
k_lc_next_x8 = 0x178a,
kloc_1af1e = 0x178b,
kloc_1af3a = 0x178c,
k_f2_draw_waves2 = 0x178d,
kloc_1af79 = 0x178e,
kloc_1af86 = 0x178f,
kloc_1af8e = 0x1790,
kloc_1afae = 0x1791,
k_init_f5_spectr = 0x1792,
kloc_1affe = 0x1793,
kloc_1b014 = 0x1794,
kloc_1b080 = 0x1795,
klocret_1b083 = 0x1796,
k_spectr_1b084 = 0x1797,
kloc_1b098 = 0x1798,
kloc_1b0cf = 0x1799,
kloc_1b0fb = 0x179a,
kloc_1b134 = 0x179b,
kloc_1b240 = 0x179c,
kloc_1b245 = 0x179d,
kloc_1b282 = 0x179e,
k_spectr_1b406 = 0x179f,
kloc_1b426 = 0x17a0,
kloc_1b440 = 0x17a1,
kloc_1b44a = 0x17a2,
kloc_1b459 = 0x17a3,
kloc_1b46d = 0x17a4,
kloc_1b4b3 = 0x17a5,
kloc_1b4cd = 0x17a6,
klocret_1b5c7 = 0x17a7,
k_f5_draw_spectr = 0x17a8,
kloc_1b5ec = 0x17a9,
kloc_1b5fc = 0x17aa,
kloc_1b610 = 0x17ab,
kloc_1b624 = 0x17ac,
kloc_1b638 = 0x17ad,
kloc_1b64c = 0x17ae,
kloc_1b660 = 0x17af,
kloc_1b674 = 0x17b0,
kloc_1b688 = 0x17b1,
kloc_1b69c = 0x17b2,
kloc_1b6b0 = 0x17b3,
kloc_1b6c4 = 0x17b4,
kloc_1b6d8 = 0x17b5,
kloc_1b6ec = 0x17b6,
kloc_1b700 = 0x17b7,
kloc_1b714 = 0x17b8,
kloc_1b728 = 0x17b9,
kloc_1b73c = 0x17ba,
kloc_1b750 = 0x17bb,
kloc_1b764 = 0x17bc,
kloc_1b778 = 0x17bd,
kloc_1b78c = 0x17be,
kloc_1b7a0 = 0x17bf,
kloc_1b7b4 = 0x17c0,
kloc_1b7c8 = 0x17c1,
kloc_1b7dc = 0x17c2,
kloc_1b7f0 = 0x17c3,
kloc_1b802 = 0x17c4,
kloc_1b814 = 0x17c5,
kloc_1b826 = 0x17c6,
kloc_1b838 = 0x17c7,
kloc_1b84a = 0x17c8,
kloc_1b85c = 0x17c9,
kloc_1b85f = 0x17ca,
kloc_1b876 = 0x17cb,
kloc_1b8bc = 0x17cc,
kloc_1b8cc = 0x17cd,
kloc_1b8e0 = 0x17ce,
kloc_1b8f4 = 0x17cf,
kloc_1b908 = 0x17d0,
kloc_1b91c = 0x17d1,
kloc_1b930 = 0x17d2,
kloc_1b944 = 0x17d3,
kloc_1b958 = 0x17d4,
kloc_1b96c = 0x17d5,
kloc_1b980 = 0x17d6,
kloc_1b994 = 0x17d7,
kloc_1b9a8 = 0x17d8,
kloc_1b9bc = 0x17d9,
kloc_1b9d0 = 0x17da,
kloc_1b9e4 = 0x17db,
kloc_1b9f8 = 0x17dc,
kloc_1ba0c = 0x17dd,
kloc_1ba20 = 0x17de,
kloc_1ba34 = 0x17df,
kloc_1ba48 = 0x17e0,
kloc_1ba5c = 0x17e1,
kloc_1ba70 = 0x17e2,
kloc_1ba84 = 0x17e3,
kloc_1ba98 = 0x17e4,
kloc_1baac = 0x17e5,
kloc_1bac0 = 0x17e6,
kloc_1bad2 = 0x17e7,
kloc_1bae4 = 0x17e8,
kloc_1baf6 = 0x17e9,
kloc_1bb08 = 0x17ea,
kloc_1bb1a = 0x17eb,
kloc_1bb2c = 0x17ec,
kloc_1bb2f = 0x17ed,
kloc_1bb46 = 0x17ee,
k_spectr_1bbc1 = 0x17ef,
kloc_1bbf4 = 0x17f0,
kloc_1bc06 = 0x17f1,
kloc_1bc0c = 0x17f2,
kloc_1bc1b = 0x17f3,
kloc_1bc24 = 0x17f4,
k_spectr_1bc2d = 0x17f5,
kloc_1bc30 = 0x17f6,
kloc_1bc38 = 0x17f7,
kloc_1bc42 = 0x17f8,
kloc_1bc5f = 0x17f9,
kloc_1bc70 = 0x17fa,
kloc_1bc87 = 0x17fb,
kloc_1bc92 = 0x17fc,
kloc_1bcab = 0x17fd,
kloc_1bcb7 = 0x17fe,
kloc_1bcc0 = 0x17ff,
kloc_1bccc = 0x1800,
kloc_1bcdf = 0x1801,
k_spectr_1bce9 = 0x1802,
kloc_1bcf1 = 0x1803,
kloc_1bcfb = 0x1804,
kloc_1bd26 = 0x1805,
kloc_1bd3e = 0x1806,
kloc_1bd56 = 0x1807,
klocret_1bd67 = 0x1808,
k_f6_draw = 0x1809,
kloc_1bd80 = 0x180a,
kloc_1bd86 = 0x180b,
kloc_1bd88 = 0x180c,
kloc_1bd95 = 0x180d,
kloc_1bd9f = 0x180e,
kloc_1bdf2 = 0x180f,
kloc_1be07 = 0x1810,
kloc_1be10 = 0x1811,
k_hex_1be39 = 0x1812,
kloc_1be43 = 0x1813,
k_message_1be77 = 0x1814,
kloc_1be85 = 0x1815,
k_draw_frame = 0x1816,
kloc_1bf11 = 0x1817,
kloc_1bf1b = 0x1818,
kloc_1bf31 = 0x1819,
kloc_1bf3a = 0x181a,
kloc_1bf53 = 0x181b,
kloc_1bf57 = 0x181c,
k_write_scr = 0x181d,
k_n1_movepos = 0x181e,
k_text_1bf69 = 0x181f,
k_n2_setcolor = 0x1820,
klocret_1bf85 = 0x1821,
kloc_1bf86 = 0x1822,
k_put_message = 0x1823,
k_put_message2 = 0x1824,
k_loadcfg = 0x1825,
kloc_1bfc9 = 0x1826,
kloc_1bfd9 = 0x1827,
kloc_1bfe3 = 0x1828,
k_getexename = 0x1829,
kloc_1c031 = 0x182a,
kloc_1c043 = 0x182b,
kloc_1c050 = 0x182c,
k_set_egasequencer = 0x182d,
k_graph_1c070 = 0x182e,
k_int9_keyb = 0x182f,
kloc_1c0a5 = 0x1830,
kloc_1c0c9 = 0x1831,
k_l_rshift = 0x1832,
k_l_rshiftup = 0x1833,
k_l_lshift = 0x1834,
k_l_lshiftup = 0x1835,
k_l_ctrl = 0x1836,
k_l_lctrlup = 0x1837,
k_l_alt = 0x1838,
k_l_altup = 0x1839,
k_l_escaped_scancode = 0x183a,
kloc_1c11f = 0x183b,
k_get_keybsw = 0x183c,
k_set_keybsw = 0x183d,
k_int24 = 0x183e,
klocret_1c159 = 0x183f,
k_int2f_checkmyself = 0x1840,
kloc_1c160 = 0x1841,
k_lyesitsme = 0x1842,
kloc_1c17c = 0x1843,
k_int1a_timer = 0x1844,
kloc_1c19c = 0x1845,
k_dosexec = 0x1846,
kloc_1c209 = 0x1847,
kloc_1c23e = 0x1848,
kloc_1c25c = 0x1849,
k_get_comspec = 0x184a,
kloc_1c273 = 0x184b,
kloc_1c28f = 0x184c,
kloc_1c299 = 0x184d,
klocret_1c29d = 0x184e,
k_find_mods = 0x184f,
kloc_1c2b6 = 0x1850,
kloc_1c2ca = 0x1851,
kloc_1c2e7 = 0x1852,
kloc_1c309 = 0x1853,
kloc_1c321 = 0x1854,
k_dosfindnext = 0x1855,
k_video_prp_mtr_positn = 0x1856,
kloc_1c355 = 0x1857,
kloc_1c365 = 0x1858,
kloc_1c369 = 0x1859,
kloc_1c37f = 0x185a,
kloc_1c396 = 0x185b,
kloc_1c3a9 = 0x185c,
kloc_1c3c1 = 0x185d,
kloc_1c3dc = 0x185e,
kloc_1c3ec = 0x185f,
kloc_1c3ee = 0x1860,
kloc_1c40b = 0x1861,
kloc_1c424 = 0x1862,
k_callsubx = 0x1863,
kloc_1c4a6 = 0x1864,
klocret_1c4a7 = 0x1865,
k_rereadrtc_settmr = 0x1866,
k_spectr_1c4f8 = 0x1867,
kloc_1c501 = 0x1868,
kloc_1c515 = 0x1869,
klocret_1c521 = 0x186a,
k_txt_blinkingoff = 0x186b,
k_txt_enableblink = 0x186c,
k_my_u32tox = 0x186d,
k_my_u16tox = 0x186e,
k_my_u8tox = 0x186f,
k_my_u4tox = 0x1870,
kloc_1c556 = 0x1871,
k_my_i8toa10 = 0x1872,
k_my_i16toa10 = 0x1873,
k_my_i32toa10 = 0x1874,
k_my_u8toa10 = 0x1875,
k_my_u16toa10 = 0x1876,
k_my_u32toa10 = 0x1877,
k_my_i32toa10_ = 0x1878,
k_my_u32toa = 0x1879,
kloc_1c58e = 0x187a,
k_myputdigit = 0x187b,
k_mystrlen = 0x187c,
kloc_1c6ab = 0x187d,
k_strcpy_count = 0x187e,
kloc_1c6b9 = 0x187f,
kloc_1c6be = 0x1880,
k_mouse_init = 0x1881,
kloc_1c6ef = 0x1882,
kloc_1c708 = 0x1883,
k_mouse_deinit = 0x1884,
klocret_1c72b = 0x1885,
kloc_1c72c = 0x1886,
k_mouse_show = 0x1887,
klocret_1c755 = 0x1888,
k_mouse_hide = 0x1889,
klocret_1c76c = 0x188a,
k_mouse_getpos = 0x188b,
kloc_1c783 = 0x188c,
k_mouse_showcur = 0x188d,
k_mouse_hide2 = 0x188e,
kloc_1c7a7 = 0x188f,
k_mouse_1c7a9 = 0x1890,
kloc_1c7af = 0x1891,
kloc_1c7b5 = 0x1892,
kloc_1c7ca = 0x1893,
kloc_1c7cc = 0x1894,
k_mouse_1c7cf = 0x1895,
kloc_1c7e9 = 0x1896,
k_int8old = 0x1897,
};

struct __attribute__((__packed__)) Memory{
 db dummy1[4096]; // protective
 db _text[16]; // segment _text
db asc_1058C[5];
db dummy2[6];
db _s3mtable_108D6[13];
db dummy3[13];
db _s3mtable_108F0[15];
db dummy4;
db _byte_11C29;
db dummy5[7];
db _byte_13C54[14];
db dummy6[2];
db _table_13EC3[16];
db _table_14057[13];
db dummy7[3];
db dummy8[2];
dw _wss_freq_table;
dw _wss_freq_table2[12];
dw dummy9[12];
dw dummy10[3];
db dummy11;
dw _word_14913;
db dummy12;
dw _word_14BBB;
db dummy13;
dw _word_14CEB;
dd _int8addr;
dw audio_len;
dw _timer_word_14F6E;
db _byte_14F70;
db _byte_14F71;
db _byte_14F72;
db _byte_14F73;
db dummy14;
dw _word_14FC0;
db dummy15;
dw _word_14FC5;
db dummy16;
dw _word_14FC8;
db dummy17;
dw _word_1504D;
db dummy18;
dw _word_15056;
db dummy19;
dw _word_15126;
db dummy20;
dw _word_1519B;
db dummy21;
db dummy22;
dw _word_151A3;
db _pc_timer_tbl[13];
db dummy23[13];
db dummy24[13];
db dummy25[13];
db dummy26[13];
db dummy27[13];
db dummy28[13];
db dummy29[13];
db dummy30[13];
db dummy31[13];
db dummy32;
db dummy33[13];
db dummy34[13];
db dummy35[16];
db dummy36[26];
db dummy37[26];
db dummy38[26];
db dummy39[5];
db dummy40;
db dummy41;
db _byte_158B4;
db dummy42;
db dummy43;
db _byte_158E3;
db dummy44;
db dummy45;
db _byte_15912;
db dummy46;
db dummy47;
db _byte_15941;
db dummy48;
db dummy49;
db _byte_15970;
db dummy50;
db dummy51;
db _byte_1599F;
db dummy52;
db dummy53;
db _byte_159CE;
db dummy54;
db dummy55;
db _byte_159FD;
db dummy56;
db dummy57;
db _byte_15A2C;
db dummy58;
db dummy59;
db _byte_15A5B;
db dummy60;
db dummy61;
db _byte_15A8A;
db dummy62;
db dummy63;
db _byte_15AB9;
db dummy64;
db dummy65;
db _byte_15AE8;
db dummy66;
db dummy67;
db _byte_15B17;
db dummy68;
db dummy69;
db _byte_15B46;
db dummy70;
db dummy71;
db _byte_15B81;
db dummy72;
db dummy73;
db _byte_15BAD;
db dummy74;
db dummy75;
db _byte_15BDA;
db dummy76;
db dummy77;
db _byte_15C07;
db dummy78;
db dummy79;
db _byte_15C34;
db dummy80;
db dummy81;
db _byte_15C61;
db dummy82;
db dummy83;
db _byte_15C8E;
db dummy84;
db dummy85;
db _byte_15CBB;
db dummy86;
db dummy87;
db _byte_15CE8;
db dummy88;
db dummy89;
db _byte_15D15;
db dummy90;
db dummy91;
db _byte_15D42;
db dummy92;
db dummy93;
db _byte_15D6F;
db dummy94;
db dummy95;
db _byte_15D9C;
db dummy96;
db dummy97;
db _byte_15DC9;
db dummy98;
db dummy99;
db _byte_15DF6;
db dummy100;
db dummy101;
db _byte_15E23;
db dummy102;
db dummy103;
db _byte_16379;
db dummy104;
db dummy105;
db _byte_163A8;
db dummy106;
db dummy107;
db _byte_163D7;
db dummy108;
db dummy109;
db _byte_16406;
db dummy110;
db dummy111;
db _byte_16435;
db dummy112;
db dummy113;
db unk_16464;
db dummy114;
db dummy115;
db _byte_16493;
db dummy116;
db dummy117;
db _byte_164C2;
db dummy118;
db dummy119;
db _byte_164F1;
db dummy120;
db dummy121;
db _byte_16520;
db dummy122;
db dummy123;
db _byte_1654F;
db dummy124;
db dummy125;
db _byte_1657E;
db dummy126;
db dummy127;
db unk_165AD;
db dummy128;
db dummy129;
db _byte_165DC;
db dummy130;
db dummy131;
db _byte_1660B;
db dummy132;
db dummy133;
db _byte_16646;
db dummy134;
db dummy135;
db _byte_16672;
db dummy136;
db dummy137;
db _byte_1669F;
db dummy138;
db dummy139;
db _byte_166CC;
db dummy140;
db dummy141;
db _byte_166F9;
db dummy142;
db dummy143;
db _byte_16726;
db dummy144;
db dummy145;
db _byte_16753;
db dummy146;
db dummy147;
db _byte_16780;
db dummy148;
db dummy149;
db _byte_167AD;
db dummy150;
db dummy151;
db _byte_167DA;
db dummy152;
db dummy153;
db _byte_16807;
db dummy154;
db dummy155;
db _byte_16834;
db dummy156;
db dummy157;
db _byte_16861;
db dummy158;
db dummy159;
db _byte_1688E;
db dummy160;
db dummy161;
db _byte_168BB;
db dummy162;
db dummy163;
db _byte_168E8;
db asc_182C3[16];
db asc_182D3[8];
dw _asmprintf_tbl;
dw dummy164;
dw dummy165;
dw dummy166;
dw dummy167;
dw dummy168;
dw dummy169;
dw dummy170;
dw dummy171;
dw dummy172;
dw dummy173;
dw dummy174;
dw dummy175;
dw _offs_noninterp2;
dw dummy176;
dw dummy177;
dw dummy178;
dw dummy179;
dw dummy180;
dw dummy181;
dw dummy182;
dw dummy183;
dw dummy184;
dw dummy185;
dw dummy186;
dw dummy187;
dw dummy188;
dw dummy189;
dw dummy190;
dw _offs_interpol2;
dw dummy191;
dw dummy192;
dw dummy193;
dw dummy194;
dw dummy195;
dw dummy196;
dw dummy197;
dw dummy198;
dw dummy199;
dw dummy200;
dw dummy201;
dw dummy202;
dw dummy203;
dw dummy204;
dw dummy205;
dw off_18E00;
dw dummy206;
dw dummy207;
dw dummy208;
dw dummy209;
dw dummy210;
dw dummy211;
dw dummy212;
dw dummy213;
dw dummy214;
dw dummy215;
dw dummy216;
dw dummy217;
dw dummy218;
dw dummy219;
dw dummy220;
dw _offs_noninterp;
dw dummy221;
dw dummy222;
dw dummy223;
dw dummy224;
dw dummy225;
dw dummy226;
dw dummy227;
dw dummy228;
dw dummy229;
dw dummy230;
dw dummy231;
dw dummy232;
dw dummy233;
dw dummy234;
dw dummy235;
dw _offs_interpol;
dw dummy236;
dw dummy237;
dw dummy238;
dw dummy239;
dw dummy240;
dw dummy241;
dw dummy242;
dw dummy243;
dw dummy244;
dw dummy245;
dw dummy246;
dw dummy247;
dw dummy248;
dw dummy249;
dw dummy250;
dw off_18E60;
dw dummy251;
dw dummy252;
dw dummy253;
dw dummy254;
dw dummy255;
dw dummy256;
dw dummy257;
dw dummy258;
dw dummy259;
dw dummy260;
dw dummy261;
dw dummy262;
dw dummy263;
dw dummy264;
dw dummy265;
dw off_18E80;
dw dummy266;
dw dummy267;
dw dummy268;
dw dummy269;
dw dummy270;
dw dummy271;
dw dummy272;
dw dummy273;
dw dummy274;
dw dummy275;
dw dummy276;
dw dummy277;
dw dummy278;
dw dummy279;
dw dummy280;
dw off_18EA0;
dw dummy281;
dw dummy282;
dw dummy283;
dw dummy284;
dw dummy285;
dw dummy286;
dw dummy287;
dw dummy288;
dw dummy289;
dw dummy290;
dw dummy291;
dw dummy292;
dw dummy293;
dw dummy294;
dw dummy295;
dw off_18EC0;
dw dummy296;
dw dummy297;
dw dummy298;
dw dummy299;
dw dummy300;
dw dummy301;
dw dummy302;
dw dummy303;
dw dummy304;
dw dummy305;
dw dummy306;
dw dummy307;
dw dummy308;
dw dummy309;
dw dummy310;
dw off_18EE0;
dw dummy311;
dw dummy312;
dw dummy313;
dw dummy314;
dw dummy315;
dw dummy316;
dw dummy317;
dw dummy318;
dw dummy319;
dw dummy320;
dw dummy321;
dw dummy322;
dw dummy323;
dw dummy324;
dw dummy325;
dw off_18F00;
dw dummy326;
dw dummy327;
dw dummy328;
dw dummy329;
dw dummy330;
dw dummy331;
dw dummy332;
dw dummy333;
dw dummy334;
dw dummy335;
dw dummy336;
dw dummy337;
dw dummy338;
dw dummy339;
dw dummy340;
dw off_18F20;
dw dummy341;
dw dummy342;
dw dummy343;
dw dummy344;
dw dummy345;
dw dummy346;
dw dummy347;
dw dummy348;
dw dummy349;
dw dummy350;
dw dummy351;
dw dummy352;
dw dummy353;
dw dummy354;
dw dummy355;
dw off_18F40;
dw dummy356;
dw dummy357;
dw dummy358;
dw dummy359;
dw dummy360;
dw dummy361;
dw dummy362;
dw dummy363;
dw dummy364;
dw dummy365;
dw dummy366;
dw dummy367;
dw dummy368;
dw dummy369;
dw dummy370;
dw _effoff_18F60;
dw dummy371;
dw dummy372;
dw dummy373;
dw dummy374;
dw dummy375;
dw dummy376;
dw dummy377;
dw dummy378;
dw dummy379;
dw dummy380;
dw dummy381;
dw dummy382;
dw dummy383;
dw dummy384;
dw dummy385;
dw dummy386;
dw dummy387;
dw dummy388;
dw dummy389;
dw dummy390;
dw dummy391;
dw dummy392;
dw dummy393;
dw dummy394;
dw dummy395;
dw dummy396;
dw dummy397;
dw dummy398;
dw dummy399;
dw dummy400;
dw dummy401;
dw dummy402;
dw _effoff_18FA2;
dw dummy403;
dw dummy404;
dw dummy405;
dw dummy406;
dw dummy407;
dw dummy408;
dw dummy409;
dw dummy410;
dw dummy411;
dw dummy412;
dw dummy413;
dw dummy414;
dw dummy415;
dw dummy416;
dw dummy417;
dw dummy418;
dw dummy419;
dw dummy420;
dw dummy421;
dw dummy422;
dw dummy423;
dw dummy424;
dw dummy425;
dw dummy426;
dw dummy427;
dw dummy428;
dw dummy429;
dw dummy430;
dw dummy431;
dw dummy432;
dw dummy433;
dw dummy434;
dw _effoff_18FE4;
dw dummy435;
dw dummy436;
dw dummy437;
dw dummy438;
dw dummy439;
dw dummy440;
dw dummy441;
dw dummy442;
dw dummy443;
dw dummy444;
dw dummy445;
dw dummy446;
dw dummy447;
dw dummy448;
dw dummy449;
dw dummy450;
dw dummy451;
dw dummy452;
dw dummy453;
dw dummy454;
dw dummy455;
dw dummy456;
dw dummy457;
dw dummy458;
dw dummy459;
dw dummy460;
dw dummy461;
dw dummy462;
dw dummy463;
dw dummy464;
dw dummy465;
dw dummy466;
dw _effoff_19026;
dw dummy467;
dw dummy468;
dw dummy469;
dw dummy470;
dw dummy471;
dw dummy472;
dw dummy473;
dw dummy474;
dw dummy475;
dw dummy476;
dw dummy477;
dw dummy478;
dw dummy479;
dw dummy480;
dw dummy481;
db dummy482;
db dummy483;
db dummy484;
db dummy485;
db dummy486;
db dummy487;
db dummy488;
db dummy489;
db dummy490;
db dummy491;
 db dummy492[15]; // padding
 db seg001[16]; // segment seg001
char _aConfigFileNotF[42];
char dummy493[3];
dw _key_code;
dw _keyb_switches;
db _prev_scan_code;
dd _oint9_1C1A4;
dd _int1Avect;
dd _oint24_1C1AC;
db dummy494[4];
dd _oint2f_1C1B4;
db _byte_1C1B8;
db dummy495[5];
 db dummy496[4]; // padding
 db dseg[16]; // segment dseg
char _aInertiaPlayerV1_[65];
db dummy497[2];
db dummy498;
char dummy499[54];
db dummy500;
char dummy501[11];
char dummy502[23];
char dummy503[34];
char dummy504[41];
char dummy505[33];
char dummy506[29];
char dummy507[40];
char dummy508[42];
char dummy509[42];
char dummy510[24];
char dummy511[24];
char dummy512[64];
db dummy513;
char dummy514[58];
char dummy515[46];
char _aCurrentSoundcard[30];
char dummy516[2];
char _myendl[3];
dw off_1CA8E;
dw dummy517;
dw dummy518;
dw dummy519;
dw dummy520;
dw dummy521;
dw _table_sndcrdname;
dw dummy522;
dw dummy523;
dw dummy524;
dw dummy525;
dw dummy526;
dw dummy527;
dw dummy528;
dw dummy529;
dw dummy530;
dw dummy531;
char _aGravisUltrasou[18];
char _aGravisMaxCodec[17];
char _aProAudioSpectr[22];
char _aWindowsSoundSy[21];
char _aSoundBlaster16[23];
char _aSoundBlasterPr[18];
char _aSoundBlaster[14];
char _aCovox[6];
char _aStereoOn1[12];
char _aAdlibSoundcard[16];
char _aPcHonker[10];
char _aGeneralMidi[13];
dw _atop_title;
db dummy532;
char _aInertiaPlayerV1_22A[62];
char dummy533[1];
db dummy534;
db dummy535;
db dummy536;
char _aCopyrightC1994[61];
db dummy537;
db dummy538;
db dummy539;
db dummy540;
db dummy541;
char _aShell130295211[24];
db dummy542;
db dummy543;
db dummy544;
char _aPlayer13029521[8];
char _a130295211558[18];
dw _bottom_menu;
db dummy545;
char asc_1CC2D[30];
db dummy546;
dw dummy547;
db dummy548;
db dummy549;
char _aFilename_0[16];
db dummy550;
db dummy551;
char _aFilename_ext[12];
db dummy552;
dw dummy553;
db dummy554;
db dummy555;
char _aModuleType_0[16];
db dummy556;
db dummy557;
char _module_type_txt[4];
db dummy558;
db dummy559;
db dummy560;
dw dummy561;
char _aChannels[15];
db dummy562;
dw dummy563;
char _aSamplesUsed[15];
db dummy564;
dw dummy565;
char _aCurrentTrack[15];
db dummy566;
dw dummy567;
char _aTrackPosition[15];
db dummy568;
dw dummy569;
char _aSpeed[5];
db dummy570;
dw dummy571;
char dummy572[1];
db dummy573;
db dummy574;
db dummy575;
dw dummy576;
char _aTab[3];
db dummy577;
dw dummy578;
db dummy579;
db _byte_1CCEB;
db dummy580;
db dummy581;
db dummy582;
char _aPlayingInStereoFree[25];
db dummy583;
dw dummy584;
db dummy585;
db dummy586;
db dummy587;
db dummy588;
db dummy589;
char _aProtracker1_0_0[15];
db dummy590;
dw dummy591;
db dummy592;
db dummy593;
char _aF9_4[3];
db dummy594;
dw dummy595;
db dummy596;
db dummy597;
db dummy598;
db dummy599;
db dummy600;
char _aIgnoreBpmChanges[19];
db dummy601;
dw dummy602;
db dummy603;
db dummy604;
char _aF10_1[4];
db dummy605;
db dummy606;
db dummy607;
dw dummy608;
db dummy609;
db dummy610;
db dummy611;
db dummy612;
db dummy613;
char _aLoopModuleWhenDone[22];
db dummy614;
dw dummy615;
db dummy616;
db dummy617;
char _aF11_1[4];
db dummy618;
db dummy619;
db dummy620;
dw dummy621;
db dummy622;
db dummy623;
db dummy624;
db dummy625;
db dummy626;
char _a24bitInterpolation[20];
db dummy627;
dw dummy628;
db dummy629;
db dummy630;
char _aF12_1[4];
db dummy631;
db dummy632;
db dummy633;
dw dummy634;
char _aMainVolume[15];
db dummy635;
dw dummy636;
db dummy637;
db dummy638;
char dummy639[3];
db dummy640;
db dummy641;
db dummy642;
dw dummy643;
char _aVolumeAmplify[15];
db dummy644;
dw dummy645;
db dummy646;
db dummy647;
char dummy648[4];
dw _f1_help_text;
db dummy649;
char _aSoYouWantedSomeHelp[24];
db dummy650;
dw dummy651;
db dummy652;
db dummy653;
char _aF2_0[3];
db dummy654;
db dummy655;
char _aGraphicalScopesOneF[40];
db dummy656;
dw dummy657;
db dummy658;
db dummy659;
char _aF3_0[3];
db dummy660;
db dummy661;
char _aRealtimeVuMeters[20];
db dummy662;
dw dummy663;
db dummy664;
db dummy665;
char _aF4_0[3];
db dummy666;
db dummy667;
char _aViewSampleNamesTwic[36];
db dummy668;
dw dummy669;
db dummy670;
db dummy671;
char _aF5_0[3];
db dummy672;
db dummy673;
char _aFastfourierFrequenc[32];
db dummy674;
dw dummy675;
db dummy676;
db dummy677;
char _aF8_0[3];
db dummy678;
db _aDosShellTypeEx;
char _aDosShellTypeExitToR[33];
db dummy679;
dw dummy680;
db dummy681;
db _aF9_1;
char _aF9_2[3];
db dummy682;
db _aProtracker1_0C;
char _aProtracker1_0Compat[37];
db dummy683;
dw dummy684;
db dummy685;
db _aF10;
char _aF10_0[4];
db dummy686;
db _aDisableBpmOnOf;
char _aDisableBpmOnOff[19];
db dummy687;
dw dummy688;
db dummy689;
db _aF11;
char _aF11_0[4];
db dummy690;
db _aLoopModule;
char _aLoopModule_0[12];
db dummy691;
dw dummy692;
db dummy693;
db _aF12;
char _aF12_0[4];
db dummy694;
db _aToggle24bitInt;
char _aToggle24bitInterpol[27];
db dummy695;
dw dummy696;
db dummy697;
db _aGray;
char _aGray_0[8];
db dummy698;
db _aDecIncVolume;
char _aDecIncVolume_0[16];
db dummy699;
dw dummy700;
db dummy701;
db dummy702;
char dummy703[3];
db dummy704;
db _aDecIncAmplify;
char _aDecIncAmplify_0[17];
db dummy705;
dw dummy706;
db dummy707;
db _aCursor;
char _aCursor_1[7];
db dummy708;
char dummy709[1];
db dummy710;
db dummy711;
db dummy712;
char _aFastErForward[18];
db dummy713;
dw dummy714;
db dummy715;
db dummy716;
char _aCursor_0[9];
db dummy717;
db dummy718;
db dummy719;
char _aFastErRewind[17];
db dummy720;
dw dummy721;
db dummy722;
db dummy723;
char _a1Thru0[8];
db dummy724;
db dummy725;
char _aMuteChannel[14];
db dummy726;
dw dummy727;
db dummy728;
db dummy729;
char _aScrolllock[10];
db dummy730;
db dummy731;
char _aLoopPattern[14];
db dummy732;
dw dummy733;
db dummy734;
db dummy735;
char _aPause[5];
db dummy736;
db dummy737;
char _aGuess___[10];
db dummy738;
dw dummy739;
db dummy740;
db dummy741;
char _aEnd[3];
db dummy742;
db dummy743;
char _aEndPattern[13];
db dummy744;
dw dummy745;
db dummy746;
db dummy747;
char _aTab_0[3];
db dummy748;
db dummy749;
char _aTogglePalNtsc[18];
dw _hopeyoulike;
db dummy750;
char _aHopeYouLikedUsingTh[25];
db dummy751;
db dummy752;
char _aInertiaPlayer[14];
db dummy753;
db dummy754;
char _aWhichIsWrittenIn[21];
db dummy755;
db dummy756;
char _a100Assembler[15];
db dummy757;
db dummy758;
db dummy759;
dw dummy760;
char _aIfYouHaveBugReports[65];
char dummy761[2];
db dummy762;
dw dummy763;
char _aInternet[11];
db dummy764;
db dummy765;
char _aSdanesarbmarvels_hack[25];
db dummy766;
db dummy767;
db dummy768;
dw dummy769;
char _aFidonet[11];
db dummy770;
db dummy771;
char _a2284116_8[11];
db dummy772;
db dummy773;
db dummy774;
dw dummy775;
char _aSendEmailTo[14];
db dummy776;
db dummy777;
char _aListserverarboliver_s[27];
db dummy778;
db dummy779;
char _aToSubscribeToOneOrB[31];
db dummy780;
dw dummy781;
char _aThe[4];
db dummy782;
db dummy783;
char _aInertiaMailinglists[20];
db dummy784;
db dummy785;
char _aAndWriteFollowingTe[42];
db dummy786;
dw dummy787;
char _aToConnectToBinaryIn[39];
db dummy788;
db dummy789;
char _aSubscribeInertiaLis[35];
db dummy790;
db dummy791;
db dummy792;
dw dummy793;
char _aToConnectToDiscussi[39];
db dummy794;
db dummy795;
char _aSubscribeInertiaTal[36];
dw _word_1D26D;
db dummy796;
char _aShellingToOperating[31];
db dummy797;
dw dummy798;
char _aType[5];
db dummy799;
db dummy800;
char _aExit[4];
db dummy801;
db dummy802;
char _aToReturnTo[14];
db dummy803;
db dummy804;
char _aInertiaPlayer_0[15];
char _msg[34];
char _aLoadingModule[15];
char _aModuleIsCorrupt[19];
char _aNotEnoughDram_0[56];
char _aNotEnoughMemo_0[49];
char _aDeleteMarkedFil[27];
char _aDeletingFile[15];
char _aFile[4];
char _aName[4];
char _a_ext[4];
db dummy805;
char _aPal[7];
char _aNtsc[7];
dw _word_1D3B0;
db dummy806;
char _aFileSelectorHelp[18];
db dummy807;
dw dummy808;
db dummy809;
db dummy810;
char _aUse[4];
db dummy811;
db dummy812;
char _aHome[4];
db dummy813;
db dummy814;
char dummy815[1];
db dummy816;
db dummy817;
char _aEnd_0[3];
db dummy818;
db dummy819;
char dummy820[1];
db dummy821;
db dummy822;
char _aPgup[4];
db dummy823;
db dummy824;
char dummy825[1];
db dummy826;
db dummy827;
char _aPgdn[4];
db dummy828;
db dummy829;
char dummy830[1];
db dummy831;
db dummy832;
db dummy833;
db dummy834;
db dummy835;
char _aAnd[5];
db dummy836;
db dummy837;
db dummy838;
db dummy839;
db dummy840;
char _aToMoveTheHighlighte[28];
db dummy841;
dw dummy842;
char _aPress[6];
db dummy843;
db dummy844;
char _aEnter[5];
db dummy845;
db dummy846;
char _aToPlayTheModuleOrSe[49];
db dummy847;
dw dummy848;
db dummy849;
db dummy850;
char _aEsc[3];
db dummy851;
db dummy852;
db dummy853;
dw dummy854;
char _aQuitIplay[10];
db dummy855;
dw dummy856;
db dummy857;
db dummy858;
char _aF1[3];
db dummy859;
db dummy860;
db dummy861;
dw dummy862;
char _aThisHelpScreenButIG[53];
db dummy863;
dw dummy864;
db dummy865;
db dummy866;
char _aF8_1[3];
db dummy867;
db dummy868;
db dummy869;
dw dummy870;
char _aDosShellTypeExitT_0[31];
db dummy871;
dw dummy872;
db dummy873;
db dummy874;
char _aF9_3[3];
db dummy875;
db dummy876;
db dummy877;
dw dummy878;
char _aToggleQuickreadingO[34];
db dummy879;
dw dummy880;
db unk_1D516;
db dummy881;
char _aDel[3];
db dummy882;
db dummy883;
db dummy884;
dw dummy885;
char _aMarkFileToDelete[19];
db dummy886;
dw dummy887;
db dummy888;
db dummy889;
char _aCtrlDel[8];
db dummy890;
db dummy891;
db dummy892;
dw dummy893;
char _aDeleteAllFilesWhich[43];
db dummy894;
dw dummy895;
db dummy896;
db dummy897;
char _aBackspace[9];
db dummy898;
db dummy899;
db dummy900;
dw dummy901;
char _aReturnToPlaymodeOnl[49];
db dummy902;
dw dummy903;
db dummy904;
db dummy905;
char _aPressAnyKeyToReturn[44];
char _aPressF1ForHelpQu[47];
dw _word_1D614;
db _byte_1D616;
char _aF9[21];
char _aHitBackspaceToRe[61];
dw _word_1D669;
db _byte_1D66B;
char _aF9_0[7];
char _aSamplename[15];
db dummy906;
char _aXpressF4ForMor[19];
db dummy907;
char _aSizeVolModeC2T[44];
db unk_1D6C3;
char _aUnused256[8];
char _a256[4];
char _a512[4];
char _a768[4];
char _a1024[5];
char _aKb[3];
char asc_1D6E0[16];
char _aPortamentoUp[16];
char _aPortamentoDown[16];
char _aTonePortamento[16];
char _aVibrato[16];
char _aPortVolslide[16];
char _aVibrVolslide[16];
char _aTremolo[16];
char _aFinePanning[16];
char _aSetSampleOfs[16];
char _aVolumeSliding[16];
char _aPositionJump[16];
char _aVolumeChange[16];
char _aPatternBreak[16];
char _aE_command[16];
char _aSetSpeedBpm[16];
char _aSetSpeed[16];
char _aFinePortaUp[16];
char _aFinePortaDown[16];
char _aFineTonePorta[16];
char _aFineVibrato[16];
char _aFineVolSlide[16];
char _aFinePortVolsl[16];
char _aFineVibrVolsl[16];
char _aSetStmSpeed[16];
char _aAutoToneporta[16];
char _aTriller[16];
char _aTremor[16];
char _aRetrigVolume[16];
char _aArpeggio[16];
char _aSetAmplify[16];
char _aFarTempo[16];
char _aFarFineTempo[16];
char _aSetFilter[16];
char _aFineslideUp[16];
char _aFineslideDown[16];
char _aGlissandoCtrl[16];
char _aVibratoControl[16];
char _aSetFinetune[16];
char _aJumpToLoop[16];
char _aTremoloControl[16];
char _aSetPanning[16];
char _aRetriggerNote[16];
char _aFinevolumeUp[16];
char _aFinevolumeDown[16];
char _aNoteCut[16];
char _aNoteDelay[16];
char _aPatternDelay[16];
char _aInvertLoop[16];
char _aSetLoopPoint[16];
char asc_1DA00[23];
char _aMute[23];
char _aMarkedToDelete[23];
char _notes[26];
char _slider[10];
char _aModuleNotFound[20];
char _aModuleLoadErro[21];
char _aNotEnoughMemor[21];
char _aListFileNotFou[23];
db _aCriticalErrorT[2];
db dummy908;
char dummy909[65];
char dummy910[11];
char dummy911[55];
db dummy912;
char dummy913[28];
char _sIplay_cfg[10];
db _buffer_1DB6C[128];
db _buffer_1DBEC;
db dummy914;
db dummy915;
db dummy916;
db dummy917;
db dummy918;
db dummy919;
db dummy920;
db dummy921;
db dummy922;
db dummy923;
db dummy924;
db dummy925;
db dummy926;
db dummy927;
db dummy928;
db dummy929;
db dummy930;
db dummy931;
db dummy932;
db dummy933;
db unk_1DC01;
db dummy934;
db dummy935;
db dummy936;
db dummy937;
db dummy938;
db dummy939;
db dummy940;
db dummy941;
db _byte_1DC0A[98];
dd _buffer_1DC6C;
db unk_1DC70;
db dummy942;
db dummy943;
db dummy944;
db dummy945;
db dummy946;
db dummy947;
db dummy948;
db dummy949;
db dummy950;
db dummy951;
db unk_1DC7B;
db _byte_1DC7C[112];
dd _dword_1DCEC;
db _cfg_buffer;
db _snd_card_type;
dw _snd_base_port_0;
db _irq_number_1;
db _dma_channel_1;
db _freq_1DCF6;
db _byte_1DCF7;
db _byte_1DCF8;
dw _configword;
db _byte_1DCFB;
db dummy952;
db _mystr[66];
db _byte_1DD3F[69];
char _a_mod_nst_669_s[56];
char _aPlaypausloop[12];
char _aJanfebmaraprmayj[39];
char _frameborder[37];
dw _oint8off_1DE14;
dw _oint8seg_1DE16;
dw _critsectpoint_off;
dw _critsectpoint_seg;
dw _swapdata_off;
dw _swapdata_seg;
dd _videomempointer;
dd _videopoint_shiftd;
dd _segfsbx_1DE28;
dd _dword_1DE2C;
dd _messagepointer;
dd _volume_1DE34;
dw _outp_freq;
dw _esseg_atstart;
dw off_1DE3C;
dw _offs_draw;
dw _offs_draw2;
dw off_1DE42;
dw _amount_of_x;
dw _word_1DE46;
dw _current_patterns;
dw _word_1DE4A;
dw _word_1DE4C;
dw _word_1DE4E;
dw _word_1DE50;
dw _word_1DE52;
dw _word_1DE54;
dw _word_1DE56;
dw _word_1DE58;
dw _word_1DE5A;
dw _word_1DE5C;
dw _word_1DE5E;
dw _word_1DE60;
dw _word_1DE62;
dw _word_1DE64;
dw _word_1DE66;
dw _fhandle_1DE68;
dw _word_1DE6A;
dw _word_1DE6C;
dw _word_1DE6E;
db _byte_1DE70;
db _byte_1DE71;
db _byte_1DE72;
db _byte_1DE73;
db _byte_1DE74;
db _byte_1DE75;
db _byte_1DE76;
db _flg_play_settings;
db _byte_1DE78;
db _byte_1DE79;
db _byte_1DE7A;
db _byte_1DE7B;
db _byte_1DE7C;
db _byte_1DE7D;
db _byte_1DE7E;
db _byte_1DE7F;
db dummy953;
db _byte_1DE81;
db _byte_1DE82;
db _byte_1DE83;
db _byte_1DE84;
db _byte_1DE85;
db _byte_1DE86;
db dummy954;
dd _dword_1DE88;
dw _mousecolumn;
dw _mouserow;
db _byte_1DE90;
db _mouse_exist_flag;
db _mouse_visible[10];
dw _x_storage[17];
dw dummy955[16];
db dummy956;
db _buffer_1[512];
db _byte_1E0E0[1979];
db _byte_1E89B[7748];
db _buffer_2[10240];
dw _buffer_1seg;
dw _buffer_2seg;
db _byte_22EE4[4096];
db unk_23EE4;
db _byte_23EE5[99];
db _byte_23F48[300];
db unk_24074;
db _byte_24075[99];
db _byte_240D8[300];
db _byte_24204[512];
db _palette_24404;
db dummy957;
db dummy958;
db dummy959;
db dummy960;
db dummy961;
db dummy962;
db dummy963;
db dummy964;
db dummy965;
db dummy966;
db dummy967;
db dummy968;
db dummy969;
db dummy970;
db dummy971;
db dummy972;
db _vga_palette[3];
db dummy973[3];
db dummy974[3];
db dummy975[3];
db dummy976[3];
db dummy977[3];
db dummy978[3];
db dummy979[3];
db dummy980[3];
db dummy981[3];
db dummy982[3];
db dummy983[3];
db dummy984[3];
db dummy985[3];
db dummy986[3];
db dummy987[3];
dw _word_24445;
dd dummy988;
dd dummy989;
dd dummy990;
db unk_24453;
db dummy991;
db dummy992;
db unk_24456;
db dummy993;
db dummy994;
db dummy995;
db dummy996;
db dummy997;
db dummy998;
db dummy999;
db dummy1000;
db dummy1001;
db dummy1002;
dw dummy1003;
db dummy1004;
db unk_244C4;
db dummy1005;
db dummy1006;
db dummy1007;
dd _dword_244C8;
dd _multip_244CC;
dd _multip_244D0;
dd _dword_244D4;
db dummy1008;
db dummy1009;
db dummy1010;
db dummy1011;
db dummy1012;
db dummy1013;
db dummy1014;
db dummy1015;
db dummy1016;
db dummy1017;
db dummy1018;
db dummy1019;
dd _dword_244E4;
dd _dword_244E8;
dd _dword_244EC;
dd _dword_244F0;
dd _dword_244F4;
dd _dword_244F8;
dd _dword_244FC;
dd _dword_24500;
dd _dword_24504;
dd _dword_24508;
dw _word_2450C;
dw _word_2450E;
db dummy1020;
db dummy1021;
db dummy1022;
db dummy1023;
dw _word_24514;
dw _word_24516;
dw _word_24518;
dw _word_2451A;
dw _word_2451C;
dw _word_2451E;
dw _word_24520;
dw _word_24522;
dw _word_24524;
dd _tabledword_24526[10];
dd dummy1024[5];
dd _tabledword_24562[9];
dd dummy1025[6];
db dummy1026;
db dummy1027;
 db dummy1028[3]; // padding
 db seg003[16]; // segment seg003
char _a070295122642[18];
dd _pointer_245B4;
dd _dma_buf_pointer;
dd _dword_245BC;
dd _dword_245C0;
dd _dword_245C4;
dw off_245C8;
dw off_245CA;
dw off_245CC;
dw off_245CE;
dd _savesp_245D0;
dw _word_245D2;
dw _mod_channels_number;
dw _word_245D6;
dw _word_245D8;
dw _word_245DA;
dw _word_245DC;
dw _freq_245DE;
dw off_245E0;
dw off_245E2;
dw _word_245E4;
dw _word_245E8;
dw _word_245EA;
dw _word_245EC;
dw _word_245EE;
dw _word_245F0;
dw _word_245F2;
dw _my_seg_index;
dw _word_245F6;
dw _word_245F8;
dw _word_245FA;
dw _volume_245FC;
dw _amplification;
dw _samples_outoffs_24600;
dw _word_24602;
dw _interrupt_mask;
dw _old_intprocoffset;
dw _old_intprocseg;
dw _intvectoffset;
dw _word_2460C;
dw _word_2460E;
dw _word_24610;
dw _my_size;
dw _word_24614;
db _byte_24616;
db _byte_24617;
db _byte_24618;
db _byte_24619;
db _byte_2461A;
db _byte_2461B;
db _byte_2461C;
db _byte_2461D;
db _byte_2461E;
db _byte_2461F;
db _byte_24620;
db _byte_24621;
db _sndflags_24622;
db _is_stereo;
db _bit_mode;
db _high_amplif;
dw _gravis_port;
db _byte_24628;
db _byte_24629;
db _irq_number_0;
db _byte_2462B;
db _dma_channel_0;
db _byte_2462D;
dw off_2462E;
dw _word_24630;
dw _word_24632;
dw _word_24634;
dw _word_24636;
dw _freq2;
db dummy1029;
db _byte_2463B;
dd _dword_2463C;
dd _dword_24640;
db dummy1030;
db _byte_24645;
dw _word_24646;
dw _sound_port;
db _byte_2464A;
db _byte_2464B;
dw _base_port2;
db _dma_channel2;
db _irq_number2;
db _byte_24650;
db _byte_24651;
dw _sb_base_port;
dw _word_24654;
dw off_24656;
db _dma_chn_mask;
db _sb_irq_number;
db _sb_timeconst;
dw _word_2465C;
dw _freq1;
dw _fhandle_module;
dw _word_24662;
db _byte_24664;
db _byte_24665;
db _byte_24666;
db _byte_24667;
db _byte_24668;
db _byte_24669;
db _byte_2466A;
db _byte_2466B;
db _byte_2466C;
db _byte_2466D;
db _byte_2466E;
db _dma_mode;
db _sb_int_counter;
db _byte_24671;
db _flag_playsetttings;
db _byte_24673;
db _byte_24674;
db _byte_24675;
db _byte_24676;
db _byte_24677;
db _byte_24678;
db _byte_24679;
db _byte_2467A;
db _byte_2467B;
db _byte_2467C;
db _byte_2467D;
db _byte_2467E;
db _play_state;
db _snd_init;
db _snd_set_flag;
db _byte_24682;
db _byte_24683;
dd _dword_24684;
db dummy1031;
dw _word_2468C;
db dummy1032;
dd _dword_24694;
dw _myseg_24698;
db _memflg_2469A;
db _byte_2469B;
db _byte_2469C;
db dummy1033;
dw _ems_pageframe;
dw _ems_handle;
dw _ems_log_pagenum;
db _ems_enabled;
db _byte_246A5;
dw _word_246A6;
db _byte_246A8;
dw _word_246A9;
dd _module_type_text;
char asc_246B0[32];
dw _moduleflag_246D0;
db _sndcard_type;
dw _snd_base_port;
db _irq_number;
db _dma_channel;
db _freq_246D7;
db _byte_246D8;
db _byte_246D9;
dw _config_word;
db _byte_246DC;
dw _word_246DE[9];
dw dummy1034[3];
dw _table_246F6[11];
dw dummy1035[5];
dw _table_24716[8];
dw dummy1036[8];
dw dummy1037[8];
dw dummy1038[8];
dw dummy1039[8];
dw dummy1040[8];
dw dummy1041[8];
dw dummy1042[8];
dw dummy1043;
dw _table_24798[8];
dw dummy1044[8];
dw dummy1045[8];
dw dummy1046[8];
dw dummy1047[8];
dw dummy1048[8];
dw dummy1049[8];
dw dummy1050[8];
dw _table_24818[8];
dw dummy1051[8];
dw dummy1052[8];
dw dummy1053[8];
dw dummy1054[8];
dw dummy1055[8];
dw dummy1056[8];
dw dummy1057[8];
db _table_24898[16];
db dummy1058[16];
db dummy1059[16];
db dummy1060[16];
db dummy1061[16];
db dummy1062[16];
db dummy1063[16];
db dummy1064[16];
db dummy1065[16];
db dummy1066[16];
db dummy1067[16];
db dummy1068[16];
db dummy1069[16];
db dummy1070[16];
db dummy1071[16];
db dummy1072[16];
dw _word_24998[9];
dw dummy1073[9];
dw dummy1074[9];
dw dummy1075[9];
dw dummy1076[9];
dw dummy1077[11];
dw dummy1078[10];
dw dummy1079[9];
dw dummy1080[9];
dw dummy1081[9];
dw dummy1082[9];
dw dummy1083[11];
dw dummy1084[10];
dw dummy1085[9];
dw dummy1086[9];
dw dummy1087[9];
dw dummy1088[9];
dw dummy1089[10];
dw dummy1090[11];
dw dummy1091[9];
dw dummy1092[9];
dw dummy1093[9];
dw dummy1094[9];
dw dummy1095[10];
dw dummy1096[11];
dw dummy1097[9];
dw dummy1098[9];
dw dummy1099[9];
dw dummy1100[9];
dw dummy1101[9];
dw dummy1102[11];
dw dummy1103[10];
dw dummy1104[9];
dw dummy1105[9];
dw dummy1106[9];
dw dummy1107[9];
dw dummy1108[10];
dw dummy1109[11];
dw dummy1110[9];
dw dummy1111[9];
dw dummy1112[9];
dw dummy1113[9];
dw dummy1114[10];
dw dummy1115[11];
dw dummy1116[9];
dw dummy1117[9];
dw dummy1118[9];
dw dummy1119[9];
dw dummy1120[9];
dw dummy1121[11];
dw dummy1122[10];
dw dummy1123[9];
dw dummy1124[9];
dw dummy1125[9];
dw dummy1126[9];
dw dummy1127[10];
dw dummy1128[11];
dw dummy1129[9];
dw dummy1130[9];
dw dummy1131[9];
dw dummy1132[9];
dw dummy1133[9];
dw dummy1134[11];
dw dummy1135[10];
dw dummy1136[9];
dw dummy1137[9];
dw dummy1138[9];
dw dummy1139[9];
dw dummy1140[11];
dw dummy1141[10];
dw dummy1142[9];
dw dummy1143[9];
dw dummy1144[9];
dw dummy1145[9];
dw dummy1146[10];
dw dummy1147[11];
dw dummy1148[9];
dw dummy1149[9];
dw dummy1150[9];
dw dummy1151[9];
dw dummy1152[9];
dw dummy1153[11];
dw dummy1154[10];
dw dummy1155[9];
dw dummy1156[9];
dw dummy1157[9];
dw dummy1158[9];
dw dummy1159[11];
dw dummy1160[10];
dw dummy1161[9];
dw dummy1162[9];
dw dummy1163[9];
dw dummy1164[9];
dw dummy1165[10];
dw dummy1166[11];
dw dummy1167[9];
dw dummy1168[9];
dw dummy1169[9];
dw dummy1170[9];
dw dummy1171[9];
dw dummy1172[11];
dw dummy1173[4];
dw _table_25118[24];
dw dummy1174[24];
dw dummy1175[24];
dw dummy1176[12];
db _table_251C0[12];
db dummy1177[11];
db dummy1178[9];
db _table_251E0[13];
db dummy1179[13];
db dummy1180[11];
db dummy1181[11];
db dummy1182[11];
db dummy1183[6];
db _table_25221[14];
db dummy1184[13];
db dummy1185[13];
db dummy1186[11];
db dummy1187[11];
db dummy1188[2];
db _table_25261[14];
db dummy1189[13];
db dummy1190[13];
db dummy1191[11];
db dummy1192[11];
db dummy1193[3];
dw dummy1194;
dw dummy1195;
dw dummy1196;
dw dummy1197;
dw dummy1198;
dw dummy1199;
dw dummy1200;
dw dummy1201;
dw dummy1202;
dw dummy1203;
dw dummy1204;
dw dummy1205;
dw dummy1206;
dw dummy1207;
dw dummy1208;
dw dummy1209;
dw dummy1210;
dw dummy1211;
dw dummy1212;
dw dummy1213;
dw dummy1214;
dw dummy1215;
dw dummy1216;
dw dummy1217;
dw _snd_cards_offs;
dw dummy1218;
dw dummy1219;
dw dummy1220;
dw dummy1221;
dw dummy1222;
dw dummy1223;
dw dummy1224;
dw dummy1225;
dw dummy1226;
dw dummy1227;
dw dummy1228;
dw dummy1229;
dw dummy1230;
dw dummy1231;
dw dummy1232;
dw dummy1233;
dw dummy1234;
dw dummy1235;
dw dummy1236;
dw dummy1237;
dw off_25326;
db dummy1238;
db dummy1239;
db dummy1240;
char _aInertiaModule_1[16];
dw dummy1241;
db dummy1242;
db dummy1243;
db dummy1244;
char _aM_k_[4];
dw dummy1245;
db dummy1246;
db dummy1247;
db dummy1248;
char _a_m_k[4];
dw dummy1249;
db dummy1250;
db dummy1251;
db dummy1252;
char _aMK[4];
dw dummy1253;
db dummy1254;
db dummy1255;
db dummy1256;
char _aMK_0[4];
dw dummy1257;
db dummy1258;
db dummy1259;
db dummy1260;
char _aGsft[4];
dw dummy1261;
db dummy1262;
db dummy1263;
db dummy1264;
char _aE_g_[4];
dw dummy1265;
db dummy1266;
db dummy1267;
db dummy1268;
char _aFlt4[4];
dw dummy1269;
db dummy1270;
db dummy1271;
db dummy1272;
char _aFlt8[4];
dw dummy1273;
db dummy1274;
db dummy1275;
db dummy1276;
char _aCd81[4];
dw dummy1277;
db dummy1278;
db dummy1279;
db dummy1280;
char _aOcta[4];
dw dummy1281;
db dummy1282;
db dummy1283;
db dummy1284;
char _aChn[3];
dw dummy1285;
db dummy1286;
db dummy1287;
db dummy1288;
char _aCh[2];
dw dummy1289;
db dummy1290;
db dummy1291;
db dummy1292;
char _aTdz[3];
dw dummy1293;
db dummy1294;
db dummy1295;
db dummy1296;
char _aScream[8];
dw dummy1297;
db dummy1298;
db dummy1299;
db dummy1300;
char _aBmod2stm[8];
dw dummy1301;
db dummy1302;
db dummy1303;
db dummy1304;
char _aScrm[4];
dw dummy1305;
db dummy1306;
db dummy1307;
db dummy1308;
char _aMtm[3];
dw dummy1309;
db dummy1310;
db dummy1311;
db dummy1312;
char _aPsm[4];
dw dummy1313;
db dummy1314;
db dummy1315;
db dummy1316;
char _aFar[4];
dw dummy1317;
db dummy1318;
db dummy1319;
db dummy1320;
char _aMas_utrack_v[12];
dw dummy1321;
db dummy1322;
db dummy1323;
db dummy1324;
char _aIf[2];
dw dummy1325;
db dummy1326;
db dummy1327;
db dummy1328;
char _aJn[2];
char _eModuleNotFound[19];
char _aNotEnoughMemory[30];
char _aNotEnoughDramOn[32];
char _aSomeFunctionsOf[47];
db dummy1329;
char dummy1330[46];
char dummy1331[51];
char _aCouldNotFindThe[49];
char _aCouldNotFindT_0[65];
char dummy1332[4];
char _aThisProgramRequ[54];
char _aErrorSoundcardN[31];
char _aErrorCouldNotFi[33];
char _aErrorCouldNot_0[29];
char _aErrorCouldNot_1[29];
char _aDeviceNotIniti[24];
char _aAt[4];
char _aBasePort[12];
char _aMixedAt[12];
char _aKhz[4];
char _aGravisUltrasoun[18];
db _gravis_txt;
db dummy1333;
dw dummy1334;
db dummy1335;
db dummy1336;
dw dummy1337;
db dummy1338;
db dummy1339;
dw dummy1340;
db dummy1341;
db dummy1342;
dw dummy1343;
char _aHGf1Irq[11];
db dummy1344;
db dummy1345;
dw dummy1346;
char _aDramDma[11];
db dummy1347;
db dummy1348;
dw dummy1349;
db dummy1350;
char _aProAudioSpectrum[22];
char _aWindowsSoundSyst[21];
char _aSoundBlaster1616[23];
char _aSoundBlasterPro[18];
char _aSoundBlaster_0[14];
db _sb16_txt;
db dummy1351;
dw dummy1352;
dw dummy1353;
db dummy1354;
db dummy1355;
dw dummy1356;
db dummy1357;
db dummy1358;
dw dummy1359;
db dummy1360;
db dummy1361;
dw dummy1362;
char _aHIrq[7];
db dummy1363;
db dummy1364;
dw dummy1365;
char _aDma[6];
db dummy1366;
db dummy1367;
dw dummy1368;
db dummy1369;
db dummy1370;
dw dummy1371;
db dummy1372;
db dummy1373;
dw dummy1374;
db dummy1375;
db dummy1376;
dw dummy1377;
db dummy1378;
char _aCovox_0[6];
char _aStereoOn1_0[12];
db _covox_txt;
db dummy1379;
dw dummy1380;
dw dummy1381;
db dummy1382;
db dummy1383;
dw dummy1384;
db dummy1385;
db dummy1386;
dw dummy1387;
db dummy1388;
db dummy1389;
dw dummy1390;
char dummy1391[1];
db dummy1392;
db dummy1393;
dw dummy1394;
db dummy1395;
db dummy1396;
dw dummy1397;
db dummy1398;
db dummy1399;
dw dummy1400;
db dummy1401;
char _aAdlibSoundcard_0[16];
char _aPcHonker_0[10];
db _pcspeaker_txt;
db dummy1402;
dw dummy1403;
dw dummy1404;
db dummy1405;
db dummy1406;
dw dummy1407;
db dummy1408;
db dummy1409;
dw dummy1410;
db dummy1411;
db dummy1412;
dw dummy1413;
db dummy1414;
char _aGeneralMidi_0[13];
db _midi_txt;
db dummy1415;
dw dummy1416;
dw dummy1417;
db dummy1418;
db dummy1419;
dw dummy1420;
db dummy1421;
db dummy1422;
dw dummy1423;
db dummy1424;
db dummy1425;
dw dummy1426;
char dummy1427[1];
db dummy1428;
db dummy1429;
db dummy1430;
dd _dword_257A0;
dw _word_257A4;
char _aInertiaModule[17];
db dummy1431;
db dummy1432;
db dummy1433;
db dummy1434;
db dummy1435;
db dummy1436;
db dummy1437;
db dummy1438;
db dummy1439;
db dummy1440;
db dummy1441;
db dummy1442;
db dummy1443;
db dummy1444;
db dummy1445;
db dummy1446;
db dummy1447;
db dummy1448;
db dummy1449;
db dummy1450;
db dummy1451;
db dummy1452;
db dummy1453;
db dummy1454;
db dummy1455;
db dummy1456;
db dummy1457;
db dummy1458;
db dummy1459;
db dummy1460;
db dummy1461;
db dummy1462;
db dummy1463;
db dummy1464;
db unk_257D9;
db _byte_257DA;
db _byte_257DB;
db _byte_257DC;
db dummy1465;
db dummy1466;
db dummy1467;
db dummy1468;
db dummy1469;
db dummy1470;
db dummy1471;
db dummy1472;
db dummy1473;
dw _word_257E6;
dw _word_257E8;
dw _word_257EA;
dw _word_257EC;
dw _word_257EE;
dw _word_257F0;
db _byte_257F2;
db _byte_257F3;
db dummy1474;
db dummy1475;
char _aInertiaModule_0[17];
db dummy1476[31];
db dummy1477;
db dummy1478;
db dummy1479;
db dummy1480;
db dummy1481;
db dummy1482;
db dummy1483;
db dummy1484;
db dummy1485;
db dummy1486;
db dummy1487;
db dummy1488;
db dummy1489;
db dummy1490;
db dummy1491;
db dummy1492;
db dummy1493;
db dummy1494;
db dummy1495;
db dummy1496;
db dummy1497;
db dummy1498;
db dummy1499;
db dummy1500;
db dummy1501;
db dummy1502;
db dummy1503;
db dummy1504;
db dummy1505;
db dummy1506;
db dummy1507;
db dummy1508;
char _aInertiaSample[16];
char asc_25856[35];
db dummy1509;
db dummy1510;
db dummy1511;
db dummy1512;
db dummy1513;
db dummy1514;
db dummy1515;
db dummy1516;
db dummy1517;
db dummy1518;
db dummy1519;
db dummy1520;
db dummy1521;
dd _dword_25886;
db dummy1522;
db _byte_2588B;
db _byte_2588C;
db _byte_2588D;
dw _word_2588E;
db dummy1523;
db dummy1524;
dd _dword_25892;
dd _dword_25896;
db dummy1525;
db dummy1526;
db dummy1527;
db dummy1528;
db dummy1529;
db dummy1530;
db dummy1531;
db dummy1532;
db dummy1533;
db dummy1534;
db dummy1535;
db dummy1536;
db unk_258A6;
db dummy1537;
db dummy1538;
db dummy1539;
db dummy1540;
db dummy1541;
db dummy1542;
db dummy1543;
db dummy1544;
db dummy1545;
db dummy1546;
db dummy1547;
db dummy1548;
db dummy1549;
db dummy1550;
db dummy1551;
db dummy1552;
db dummy1553;
db dummy1554;
db dummy1555;
db dummy1556;
db dummy1557;
db dummy1558;
db dummy1559;
db dummy1560;
db dummy1561;
db dummy1562;
db dummy1563;
db dummy1564;
db dummy1565;
db dummy1566;
db dummy1567;
db dummy1568;
db dummy1569;
db dummy1570;
db dummy1571;
db dummy1572;
db dummy1573;
db dummy1574;
db dummy1575;
db dummy1576;
db dummy1577;
db dummy1578;
db dummy1579;
db dummy1580;
db dummy1581;
db dummy1582;
db dummy1583;
db dummy1584;
db dummy1585;
db dummy1586;
db dummy1587;
db dummy1588;
db dummy1589;
db dummy1590;
db dummy1591;
db dummy1592;
db dummy1593;
db dummy1594;
db dummy1595;
db dummy1596;
db dummy1597;
db dummy1598;
db dummy1599;
db dummy1600;
db dummy1601;
db dummy1602;
db dummy1603;
db dummy1604;
db dummy1605;
db dummy1606;
db dummy1607;
db dummy1608;
db dummy1609;
db dummy1610;
db dummy1611;
db dummy1612;
db dummy1613;
db dummy1614;
db dummy1615;
db dummy1616;
db dummy1617;
db dummy1618;
db dummy1619;
db dummy1620;
db dummy1621;
db dummy1622;
db dummy1623;
db dummy1624;
db dummy1625;
db dummy1626;
db dummy1627;
db dummy1628;
db dummy1629;
db dummy1630;
db dummy1631;
db dummy1632;
db dummy1633;
db _channels_25908[32*80]; // 32 channels
db _myout[6336];
dd _dword_27BC8;
dd _dword_27BCC;
db dummy1634[24];
dw _segs_table[256];
dw _myseg_size[256];
db _byte_27FE8[255];
db _byte_280E7;
db _byte_280E8[256];
db _byte_281E8[256];
db _byte_282E8[32];
db _vlm_byte_table[33280];
dd _chrin;
dd _myin;
db dummy1635;
db dummy1636;
db dummy1637;
db dummy1638;
db dummy1639;
dw _word_30515;
db _myin_0;
dd _dword_30518;
db dummy1640;
db dummy1641;
db dummy1642;
db dummy1643;
dw _word_30520;
db _byte_30522;
db _byte_30523;
dw _word_30524;
db _byte_30526;
db dummy1644;
db unk_30528;
db _byte_30529;
dw _word_3052A;
dw _word_3052C;
db dummy1645;
db dummy1646;
db dummy1647;
db dummy1648;
dw _word_30532;
db dummy1649;
db dummy1650;
db dummy1651;
db _byte_30537;
db _my_in;
db _byte_30539;
db _byte_3053A;
db _byte_3053B;
db dummy1652;
db dummy1653;
db dummy1654;
db dummy1655;
db dummy1656;
db dummy1657;
db dummy1658;
db dummy1659;
db dummy1660;
db dummy1661;
db dummy1662;
db dummy1663;
db _byte_30548;
db dummy1664;
db unk_3054A;
db _byte_3054B;
db _byte_3054C;
db dummy1665;
db dummy1666;
db dummy1667;
db _byte_30550;
db dummy1668;
dw _word_30552;
dw _word_30554;
dw _word_30556;
db dummy1669;
db dummy1670;
dd _dword_3055A;
db dummy1671;
db dummy1672;
db dummy1673;
db dummy1674;
dw _word_30562;
dw _word_30564;
dd _dword_30566;
db dummy1675;
db dummy1676;
db dummy1677;
db dummy1678;
db dummy1679;
db dummy1680;
db dummy1681;
db dummy1682;
db dummy1683;
db dummy1684;
db dummy1685;
db dummy1686;
db _byte_30576;
db _byte_30577;
db dummy1687;
db _byte_30579[33];
db _byte_3059A[95];
db _byte_305F9[64];
db _byte_30639;
db _byte_3063A;
dw _word_3063B;
dd _dword_3063D;
db _byte_30641[40];
db _byte_30669;
db _byte_3066A;
db _byte_3066B[14];
db _byte_30679[101];
db _byte_306DE[480];
db _byte_308BE[74];
db _byte_30908[56];
db _byte_30940;
db unk_30941;
db dummy1688;
db _byte_30943;
db dummy1689[3012];
dw _word_31508;
db _byte_3150A;
db dummy1690;
db _byte_3150C[2044];
db _byte_31D08[6144];
db _byte_33508[4104];
 db dummy1691[8]; // padding
// db seg004[16]; // segment seg004
//db _byte_34510[4096];
db stack[STACK_SIZE];
db int8stack[STACK_SIZE];

			db heap[HEAP_SIZE];
		};

//class iplay_masm_Context {
//public:
//	iplay_masm_Context() {}

//	void _start();
//};

//} // End of namespace DreamGen

#endif
