// Copyright (C) 2013       Hykem <hykem@hotmail.com>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/gpl-2.0.txt

#include "ps4.h"
#include "defines.h"
#include "debug.h"
#include "unpkg.h"

// Helper functions.
static inline uint16_t bswap_16(uint16_t val)
{
  return ((val & (uint16_t)0x00ffU) << 8)
    | ((val & (uint16_t)0xff00U) >> 8);
}

static inline uint32_t bswap_32(uint32_t val)
{
  return ((val & (uint32_t)0x000000ffUL) << 24)
    | ((val & (uint32_t)0x0000ff00UL) <<  8)
    | ((val & (uint32_t)0x00ff0000UL) >>  8)
    | ((val & (uint32_t)0xff000000UL) >> 24);
}

int fgetc(FILE *fp)
{
  char c;

  if (fread(&c, 1, 1, fp) == 0)
    return ('\00');
  return (c);
}

char *read_string(FILE* f)
{
  char *string = malloc(sizeof(char) * 256);
  int c;
  int length = 0;
  if (!string) return string;
  while((c = fgetc(f)) != '\00')
  {
    string[length++] = c;
  }
  string[length++] = '\0';

  return realloc(string, sizeof(char) * length);
}

static void _mkdir(const char *dir)
{
  char tmp[256];
  char *p = NULL;

  snprintf(tmp, sizeof(tmp), "%s", dir);
  for (p = tmp + 1; *p; p++)
  {
    if (*p == '/')
    {
      *p = 0;
      mkdir(tmp, 0777);
      *p = '/';
    }
  }
}

typedef struct
{
  uint32_t type;
  char *name;
} pkg_entry_value;

char *get_entry_name_by_type(uint32_t type)
{
  static const pkg_entry_value entries [] = {
	{ PS4_PKG_ENTRY_TYPE_DIGEST_TABLE, NULL },
	{ PS4_PKG_ENTRY_TYPE_0x800,        NULL },
	{ PS4_PKG_ENTRY_TYPE_0x200,        NULL },
	{ PS4_PKG_ENTRY_TYPE_0x180,        NULL },
	{ PS4_PKG_ENTRY_TYPE_META_TABLE,   NULL },
	{ PS4_PKG_ENTRY_TYPE_NAME_TABLE,   NULL },

	{ 0x0400, "license.dat" },
	{ 0x0401, "license.info" },
	{ 0x0402, "nptitle.dat" },
	{ 0x0403, "npbind.dat" },
	{ 0x0404, "selfinfo.dat" },
	{ 0x0406, "imageinfo.dat" },
	{ 0x0407, "target-deltainfo.dat" },
	{ 0x0408, "origin-deltainfo.dat" },
	{ 0x0409, "psreserved.dat" },
	{ 0x1000, "param.sfo" },
	{ 0x1001, "playgo-chunk.dat" },
	{ 0x1002, "playgo-chunk.sha" },
	{ 0x1003, "playgo-manifest.xml" },
	{ 0x1004, "pronunciation.xml" },
	{ 0x1005, "pronunciation.sig" },
	{ 0x1006, "pic1.png" },
	{ 0x1007, "pubtoolinfo.dat" },
	{ 0x1008, "app/playgo-chunk.dat" },
	{ 0x1009, "app/playgo-chunk.sha" },
	{ 0x100A, "app/playgo-manifest.xml" },
	{ 0x100B, "shareparam.json" },
	{ 0x100C, "shareoverlayimage.png" },
	{ 0x100D, "save_data.png" },
	{ 0x100E, "shareprivacyguardimage.png" },
	{ 0x1200, "icon0.png" },
	{ 0x1201, "icon0_00.png" },
	{ 0x1202, "icon0_01.png" },
	{ 0x1203, "icon0_02.png" },
	{ 0x1204, "icon0_03.png" },
	{ 0x1205, "icon0_04.png" },
	{ 0x1206, "icon0_05.png" },
	{ 0x1207, "icon0_06.png" },
	{ 0x1208, "icon0_07.png" },
	{ 0x1209, "icon0_08.png" },
	{ 0x120A, "icon0_09.png" },
	{ 0x120B, "icon0_10.png" },
	{ 0x120C, "icon0_11.png" },
	{ 0x120D, "icon0_12.png" },
	{ 0x120E, "icon0_13.png" },
	{ 0x120F, "icon0_14.png" },
	{ 0x1210, "icon0_15.png" },
	{ 0x1211, "icon0_16.png" },
	{ 0x1212, "icon0_17.png" },
	{ 0x1213, "icon0_18.png" },
	{ 0x1214, "icon0_19.png" },
	{ 0x1215, "icon0_20.png" },
	{ 0x1216, "icon0_21.png" },
	{ 0x1217, "icon0_22.png" },
	{ 0x1218, "icon0_23.png" },
	{ 0x1219, "icon0_24.png" },
	{ 0x121A, "icon0_25.png" },
	{ 0x121B, "icon0_26.png" },
	{ 0x121C, "icon0_27.png" },
	{ 0x121D, "icon0_28.png" },
	{ 0x121E, "icon0_29.png" },
	{ 0x121F, "icon0_30.png" },
	{ 0x1220, "pic0.png" },
	{ 0x1240, "snd0.at9" },
	{ 0x1241, "pic1_00.png" },
	{ 0x1242, "pic1_01.png" },
	{ 0x1243, "pic1_02.png" },
	{ 0x1244, "pic1_03.png" },
	{ 0x1245, "pic1_04.png" },
	{ 0x1246, "pic1_05.png" },
	{ 0x1247, "pic1_06.png" },
	{ 0x1248, "pic1_07.png" },
	{ 0x1249, "pic1_08.png" },
	{ 0x124A, "pic1_09.png" },
	{ 0x124B, "pic1_10.png" },
	{ 0x124C, "pic1_11.png" },
	{ 0x124D, "pic1_12.png" },
	{ 0x124E, "pic1_13.png" },
	{ 0x124F, "pic1_14.png" },
	{ 0x1250, "pic1_15.png" },
	{ 0x1251, "pic1_16.png" },
	{ 0x1252, "pic1_17.png" },
	{ 0x1253, "pic1_18.png" },
	{ 0x1254, "pic1_19.png" },
	{ 0x1255, "pic1_20.png" },
	{ 0x1256, "pic1_21.png" },
	{ 0x1257, "pic1_22.png" },
	{ 0x1258, "pic1_23.png" },
	{ 0x1259, "pic1_24.png" },
	{ 0x125A, "pic1_25.png" },
	{ 0x125B, "pic1_26.png" },
	{ 0x125C, "pic1_27.png" },
	{ 0x125D, "pic1_28.png" },
	{ 0x125E, "pic1_29.png" },
	{ 0x125F, "pic1_30.png" },
	{ 0x1260, "changeinfo/changeinfo.xml" },
	{ 0x1261, "changeinfo/changeinfo_00.xml" },
	{ 0x1262, "changeinfo/changeinfo_01.xml" },
	{ 0x1263, "changeinfo/changeinfo_02.xml" },
	{ 0x1264, "changeinfo/changeinfo_03.xml" },
	{ 0x1265, "changeinfo/changeinfo_04.xml" },
	{ 0x1266, "changeinfo/changeinfo_05.xml" },
	{ 0x1267, "changeinfo/changeinfo_06.xml" },
	{ 0x1268, "changeinfo/changeinfo_07.xml" },
	{ 0x1269, "changeinfo/changeinfo_08.xml" },
	{ 0x126A, "changeinfo/changeinfo_09.xml" },
	{ 0x126B, "changeinfo/changeinfo_10.xml" },
	{ 0x126C, "changeinfo/changeinfo_11.xml" },
	{ 0x126D, "changeinfo/changeinfo_12.xml" },
	{ 0x126E, "changeinfo/changeinfo_13.xml" },
	{ 0x126F, "changeinfo/changeinfo_14.xml" },
	{ 0x1270, "changeinfo/changeinfo_15.xml" },
	{ 0x1271, "changeinfo/changeinfo_16.xml" },
	{ 0x1272, "changeinfo/changeinfo_17.xml" },
	{ 0x1273, "changeinfo/changeinfo_18.xml" },
	{ 0x1274, "changeinfo/changeinfo_19.xml" },
	{ 0x1275, "changeinfo/changeinfo_20.xml" },
	{ 0x1276, "changeinfo/changeinfo_21.xml" },
	{ 0x1277, "changeinfo/changeinfo_22.xml" },
	{ 0x1278, "changeinfo/changeinfo_23.xml" },
	{ 0x1279, "changeinfo/changeinfo_24.xml" },
	{ 0x127A, "changeinfo/changeinfo_25.xml" },
	{ 0x127B, "changeinfo/changeinfo_26.xml" },
	{ 0x127C, "changeinfo/changeinfo_27.xml" },
	{ 0x127D, "changeinfo/changeinfo_28.xml" },
	{ 0x127E, "changeinfo/changeinfo_29.xml" },
	{ 0x127F, "changeinfo/changeinfo_30.xml" },
	{ 0x1280, "icon0.dds" },
	{ 0x1281, "icon0_00.dds" },
	{ 0x1282, "icon0_01.dds" },
	{ 0x1283, "icon0_02.dds" },
	{ 0x1284, "icon0_03.dds" },
	{ 0x1285, "icon0_04.dds" },
	{ 0x1286, "icon0_05.dds" },
	{ 0x1287, "icon0_06.dds" },
	{ 0x1288, "icon0_07.dds" },
	{ 0x1289, "icon0_08.dds" },
	{ 0x128A, "icon0_09.dds" },
	{ 0x128B, "icon0_10.dds" },
	{ 0x128C, "icon0_11.dds" },
	{ 0x128D, "icon0_12.dds" },
	{ 0x128E, "icon0_13.dds" },
	{ 0x128F, "icon0_14.dds" },
	{ 0x1290, "icon0_15.dds" },
	{ 0x1291, "icon0_16.dds" },
	{ 0x1292, "icon0_17.dds" },
	{ 0x1293, "icon0_18.dds" },
	{ 0x1294, "icon0_19.dds" },
	{ 0x1295, "icon0_20.dds" },
	{ 0x1296, "icon0_21.dds" },
	{ 0x1297, "icon0_22.dds" },
	{ 0x1298, "icon0_23.dds" },
	{ 0x1299, "icon0_24.dds" },
	{ 0x129A, "icon0_25.dds" },
	{ 0x129B, "icon0_26.dds" },
	{ 0x129C, "icon0_27.dds" },
	{ 0x129D, "icon0_28.dds" },
	{ 0x129E, "icon0_29.dds" },
	{ 0x129F, "icon0_30.dds" },
	{ 0x12A0, "pic0.dds" },
	{ 0x12C0, "pic1.dds" },
	{ 0x12C1, "pic1_00.dds" },
	{ 0x12C2, "pic1_01.dds" },
	{ 0x12C3, "pic1_02.dds" },
	{ 0x12C4, "pic1_03.dds" },
	{ 0x12C5, "pic1_04.dds" },
	{ 0x12C6, "pic1_05.dds" },
	{ 0x12C7, "pic1_06.dds" },
	{ 0x12C8, "pic1_07.dds" },
	{ 0x12C9, "pic1_08.dds" },
	{ 0x12CA, "pic1_09.dds" },
	{ 0x12CB, "pic1_10.dds" },
	{ 0x12CC, "pic1_11.dds" },
	{ 0x12CD, "pic1_12.dds" },
	{ 0x12CE, "pic1_13.dds" },
	{ 0x12CF, "pic1_14.dds" },
	{ 0x12D0, "pic1_15.dds" },
	{ 0x12D1, "pic1_16.dds" },
	{ 0x12D2, "pic1_17.dds" },
	{ 0x12D3, "pic1_18.dds" },
	{ 0x12D4, "pic1_19.dds" },
	{ 0x12D5, "pic1_20.dds" },
	{ 0x12D6, "pic1_21.dds" },
	{ 0x12D7, "pic1_22.dds" },
	{ 0x12D8, "pic1_23.dds" },
	{ 0x12D9, "pic1_24.dds" },
	{ 0x12DA, "pic1_25.dds" },
	{ 0x12DB, "pic1_26.dds" },
	{ 0x12DC, "pic1_27.dds" },
	{ 0x12DD, "pic1_28.dds" },
	{ 0x12DE, "pic1_29.dds" },
	{ 0x12DF, "pic1_30.dds" },
	{ 0x1400, "trophy/trophy00.trp" },
	{ 0x1401, "trophy/trophy01.trp" },
	{ 0x1402, "trophy/trophy02.trp" },
	{ 0x1403, "trophy/trophy03.trp" },
	{ 0x1404, "trophy/trophy04.trp" },
	{ 0x1405, "trophy/trophy05.trp" },
	{ 0x1406, "trophy/trophy06.trp" },
	{ 0x1407, "trophy/trophy07.trp" },
	{ 0x1408, "trophy/trophy08.trp" },
	{ 0x1409, "trophy/trophy09.trp" },
	{ 0x140A, "trophy/trophy10.trp" },
	{ 0x140B, "trophy/trophy11.trp" },
	{ 0x140C, "trophy/trophy12.trp" },
	{ 0x140D, "trophy/trophy13.trp" },
	{ 0x140E, "trophy/trophy14.trp" },
	{ 0x140F, "trophy/trophy15.trp" },
	{ 0x1410, "trophy/trophy16.trp" },
	{ 0x1411, "trophy/trophy17.trp" },
	{ 0x1412, "trophy/trophy18.trp" },
	{ 0x1413, "trophy/trophy19.trp" },
	{ 0x1414, "trophy/trophy20.trp" },
	{ 0x1415, "trophy/trophy21.trp" },
	{ 0x1416, "trophy/trophy22.trp" },
	{ 0x1417, "trophy/trophy23.trp" },
	{ 0x1418, "trophy/trophy24.trp" },
	{ 0x1419, "trophy/trophy25.trp" },
	{ 0x141A, "trophy/trophy26.trp" },
	{ 0x141B, "trophy/trophy27.trp" },
	{ 0x141C, "trophy/trophy28.trp" },
	{ 0x141D, "trophy/trophy29.trp" },
	{ 0x141E, "trophy/trophy30.trp" },
	{ 0x141F, "trophy/trophy31.trp" },
	{ 0x1420, "trophy/trophy32.trp" },
	{ 0x1421, "trophy/trophy33.trp" },
	{ 0x1422, "trophy/trophy34.trp" },
	{ 0x1423, "trophy/trophy35.trp" },
	{ 0x1424, "trophy/trophy36.trp" },
	{ 0x1425, "trophy/trophy37.trp" },
	{ 0x1426, "trophy/trophy38.trp" },
	{ 0x1427, "trophy/trophy39.trp" },
	{ 0x1428, "trophy/trophy40.trp" },
	{ 0x1429, "trophy/trophy41.trp" },
	{ 0x142A, "trophy/trophy42.trp" },
	{ 0x142B, "trophy/trophy43.trp" },
	{ 0x142C, "trophy/trophy44.trp" },
	{ 0x142D, "trophy/trophy45.trp" },
	{ 0x142E, "trophy/trophy46.trp" },
	{ 0x142F, "trophy/trophy47.trp" },
	{ 0x1430, "trophy/trophy48.trp" },
	{ 0x1431, "trophy/trophy49.trp" },
	{ 0x1432, "trophy/trophy50.trp" },
	{ 0x1433, "trophy/trophy51.trp" },
	{ 0x1434, "trophy/trophy52.trp" },
	{ 0x1435, "trophy/trophy53.trp" },
	{ 0x1436, "trophy/trophy54.trp" },
	{ 0x1437, "trophy/trophy55.trp" },
	{ 0x1438, "trophy/trophy56.trp" },
	{ 0x1439, "trophy/trophy57.trp" },
	{ 0x143A, "trophy/trophy58.trp" },
	{ 0x143B, "trophy/trophy59.trp" },
	{ 0x143C, "trophy/trophy60.trp" },
	{ 0x143D, "trophy/trophy61.trp" },
	{ 0x143E, "trophy/trophy62.trp" },
	{ 0x143F, "trophy/trophy63.trp" },
	{ 0x1440, "trophy/trophy64.trp" },
	{ 0x1441, "trophy/trophy65.trp" },
	{ 0x1442, "trophy/trophy66.trp" },
	{ 0x1443, "trophy/trophy67.trp" },
	{ 0x1444, "trophy/trophy68.trp" },
	{ 0x1445, "trophy/trophy69.trp" },
	{ 0x1446, "trophy/trophy70.trp" },
	{ 0x1447, "trophy/trophy71.trp" },
	{ 0x1448, "trophy/trophy72.trp" },
	{ 0x1449, "trophy/trophy73.trp" },
	{ 0x144A, "trophy/trophy74.trp" },
	{ 0x144B, "trophy/trophy75.trp" },
	{ 0x144C, "trophy/trophy76.trp" },
	{ 0x144D, "trophy/trophy77.trp" },
	{ 0x144E, "trophy/trophy78.trp" },
	{ 0x144F, "trophy/trophy79.trp" },
	{ 0x1450, "trophy/trophy80.trp" },
	{ 0x1451, "trophy/trophy81.trp" },
	{ 0x1452, "trophy/trophy82.trp" },
	{ 0x1453, "trophy/trophy83.trp" },
	{ 0x1454, "trophy/trophy84.trp" },
	{ 0x1455, "trophy/trophy85.trp" },
	{ 0x1456, "trophy/trophy86.trp" },
	{ 0x1457, "trophy/trophy87.trp" },
	{ 0x1458, "trophy/trophy88.trp" },
	{ 0x1459, "trophy/trophy89.trp" },
	{ 0x145A, "trophy/trophy90.trp" },
	{ 0x145B, "trophy/trophy91.trp" },
	{ 0x145C, "trophy/trophy92.trp" },
	{ 0x145D, "trophy/trophy93.trp" },
	{ 0x145E, "trophy/trophy94.trp" },
	{ 0x145F, "trophy/trophy95.trp" },
	{ 0x1460, "trophy/trophy96.trp" },
	{ 0x1461, "trophy/trophy97.trp" },
	{ 0x1462, "trophy/trophy98.trp" },
	{ 0x1463, "trophy/trophy99.trp" },
	{ 0x1600, "keymap_rp/001.png" },
	{ 0x1601, "keymap_rp/002.png" },
	{ 0x1602, "keymap_rp/003.png" },
	{ 0x1603, "keymap_rp/004.png" },
	{ 0x1604, "keymap_rp/005.png" },
	{ 0x1605, "keymap_rp/006.png" },
	{ 0x1606, "keymap_rp/007.png" },
	{ 0x1607, "keymap_rp/008.png" },
	{ 0x1608, "keymap_rp/009.png" },
	{ 0x1609, "keymap_rp/010.png" },
	{ 0x1610, "keymap_rp/00/001.png" },
	{ 0x1611, "keymap_rp/00/002.png" },
	{ 0x1612, "keymap_rp/00/003.png" },
	{ 0x1613, "keymap_rp/00/004.png" },
	{ 0x1614, "keymap_rp/00/005.png" },
	{ 0x1615, "keymap_rp/00/006.png" },
	{ 0x1616, "keymap_rp/00/007.png" },
	{ 0x1617, "keymap_rp/00/008.png" },
	{ 0x1618, "keymap_rp/00/009.png" },
	{ 0x1619, "keymap_rp/00/010.png" },
	{ 0x1620, "keymap_rp/01/001.png" },
	{ 0x1621, "keymap_rp/01/002.png" },
	{ 0x1622, "keymap_rp/01/003.png" },
	{ 0x1623, "keymap_rp/01/004.png" },
	{ 0x1624, "keymap_rp/01/005.png" },
	{ 0x1625, "keymap_rp/01/006.png" },
	{ 0x1626, "keymap_rp/01/007.png" },
	{ 0x1627, "keymap_rp/01/008.png" },
	{ 0x1628, "keymap_rp/01/009.png" },
	{ 0x1629, "keymap_rp/01/010.png" },
	{ 0x1630, "keymap_rp/02/001.png" },
	{ 0x1631, "keymap_rp/02/002.png" },
	{ 0x1632, "keymap_rp/02/003.png" },
	{ 0x1633, "keymap_rp/02/004.png" },
	{ 0x1634, "keymap_rp/02/005.png" },
	{ 0x1635, "keymap_rp/02/006.png" },
	{ 0x1636, "keymap_rp/02/007.png" },
	{ 0x1637, "keymap_rp/02/008.png" },
	{ 0x1638, "keymap_rp/02/009.png" },
	{ 0x1639, "keymap_rp/02/010.png" },
	{ 0x1640, "keymap_rp/03/001.png" },
	{ 0x1641, "keymap_rp/03/002.png" },
	{ 0x1642, "keymap_rp/03/003.png" },
	{ 0x1643, "keymap_rp/03/004.png" },
	{ 0x1644, "keymap_rp/03/005.png" },
	{ 0x1645, "keymap_rp/03/006.png" },
	{ 0x1646, "keymap_rp/03/007.png" },
	{ 0x1647, "keymap_rp/03/008.png" },
	{ 0x1648, "keymap_rp/03/009.png" },
	{ 0x1649, "keymap_rp/03/010.png" },
	{ 0x1650, "keymap_rp/04/001.png" },
	{ 0x1651, "keymap_rp/04/002.png" },
	{ 0x1652, "keymap_rp/04/003.png" },
	{ 0x1653, "keymap_rp/04/004.png" },
	{ 0x1654, "keymap_rp/04/005.png" },
	{ 0x1655, "keymap_rp/04/006.png" },
	{ 0x1656, "keymap_rp/04/007.png" },
	{ 0x1657, "keymap_rp/04/008.png" },
	{ 0x1658, "keymap_rp/04/009.png" },
	{ 0x1659, "keymap_rp/04/010.png" },
	{ 0x1660, "keymap_rp/05/001.png" },
	{ 0x1661, "keymap_rp/05/002.png" },
	{ 0x1662, "keymap_rp/05/003.png" },
	{ 0x1663, "keymap_rp/05/004.png" },
	{ 0x1664, "keymap_rp/05/005.png" },
	{ 0x1665, "keymap_rp/05/006.png" },
	{ 0x1666, "keymap_rp/05/007.png" },
	{ 0x1667, "keymap_rp/05/008.png" },
	{ 0x1668, "keymap_rp/05/009.png" },
	{ 0x1669, "keymap_rp/05/010.png" },
	{ 0x1670, "keymap_rp/06/001.png" },
	{ 0x1671, "keymap_rp/06/002.png" },
	{ 0x1672, "keymap_rp/06/003.png" },
	{ 0x1673, "keymap_rp/06/004.png" },
	{ 0x1674, "keymap_rp/06/005.png" },
	{ 0x1675, "keymap_rp/06/006.png" },
	{ 0x1676, "keymap_rp/06/007.png" },
	{ 0x1677, "keymap_rp/06/008.png" },
	{ 0x1678, "keymap_rp/06/009.png" },
	{ 0x1679, "keymap_rp/06/010.png" },
	{ 0x1680, "keymap_rp/07/001.png" },
	{ 0x1681, "keymap_rp/07/002.png" },
	{ 0x1682, "keymap_rp/07/003.png" },
	{ 0x1683, "keymap_rp/07/004.png" },
	{ 0x1684, "keymap_rp/07/005.png" },
	{ 0x1685, "keymap_rp/07/006.png" },
	{ 0x1686, "keymap_rp/07/007.png" },
	{ 0x1687, "keymap_rp/07/008.png" },
	{ 0x1688, "keymap_rp/07/009.png" },
	{ 0x1689, "keymap_rp/07/010.png" },
	{ 0x1690, "keymap_rp/08/001.png" },
	{ 0x1691, "keymap_rp/08/002.png" },
	{ 0x1692, "keymap_rp/08/003.png" },
	{ 0x1693, "keymap_rp/08/004.png" },
	{ 0x1694, "keymap_rp/08/005.png" },
	{ 0x1695, "keymap_rp/08/006.png" },
	{ 0x1696, "keymap_rp/08/007.png" },
	{ 0x1697, "keymap_rp/08/008.png" },
	{ 0x1698, "keymap_rp/08/009.png" },
	{ 0x1699, "keymap_rp/08/010.png" },
	{ 0x16A0, "keymap_rp/09/001.png" },
	{ 0x16A1, "keymap_rp/09/002.png" },
	{ 0x16A2, "keymap_rp/09/003.png" },
	{ 0x16A3, "keymap_rp/09/004.png" },
	{ 0x16A4, "keymap_rp/09/005.png" },
	{ 0x16A5, "keymap_rp/09/006.png" },
	{ 0x16A6, "keymap_rp/09/007.png" },
	{ 0x16A7, "keymap_rp/09/008.png" },
	{ 0x16A8, "keymap_rp/09/009.png" },
	{ 0x16A9, "keymap_rp/09/010.png" },
	{ 0x16B0, "keymap_rp/10/001.png" },
	{ 0x16B1, "keymap_rp/10/002.png" },
	{ 0x16B2, "keymap_rp/10/003.png" },
	{ 0x16B3, "keymap_rp/10/004.png" },
	{ 0x16B4, "keymap_rp/10/005.png" },
	{ 0x16B5, "keymap_rp/10/006.png" },
	{ 0x16B6, "keymap_rp/10/007.png" },
	{ 0x16B7, "keymap_rp/10/008.png" },
	{ 0x16B8, "keymap_rp/10/009.png" },
	{ 0x16B9, "keymap_rp/10/010.png" },
	{ 0x16C0, "keymap_rp/11/001.png" },
	{ 0x16C1, "keymap_rp/11/002.png" },
	{ 0x16C2, "keymap_rp/11/003.png" },
	{ 0x16C3, "keymap_rp/11/004.png" },
	{ 0x16C4, "keymap_rp/11/005.png" },
	{ 0x16C5, "keymap_rp/11/006.png" },
	{ 0x16C6, "keymap_rp/11/007.png" },
	{ 0x16C7, "keymap_rp/11/008.png" },
	{ 0x16C8, "keymap_rp/11/009.png" },
	{ 0x16C9, "keymap_rp/11/010.png" },
	{ 0x16D0, "keymap_rp/12/001.png" },
	{ 0x16D1, "keymap_rp/12/002.png" },
	{ 0x16D2, "keymap_rp/12/003.png" },
	{ 0x16D3, "keymap_rp/12/004.png" },
	{ 0x16D4, "keymap_rp/12/005.png" },
	{ 0x16D5, "keymap_rp/12/006.png" },
	{ 0x16D6, "keymap_rp/12/007.png" },
	{ 0x16D7, "keymap_rp/12/008.png" },
	{ 0x16D8, "keymap_rp/12/009.png" },
	{ 0x16D9, "keymap_rp/12/010.png" },
	{ 0x16E0, "keymap_rp/13/001.png" },
	{ 0x16E1, "keymap_rp/13/002.png" },
	{ 0x16E2, "keymap_rp/13/003.png" },
	{ 0x16E3, "keymap_rp/13/004.png" },
	{ 0x16E4, "keymap_rp/13/005.png" },
	{ 0x16E5, "keymap_rp/13/006.png" },
	{ 0x16E6, "keymap_rp/13/007.png" },
	{ 0x16E7, "keymap_rp/13/008.png" },
	{ 0x16E8, "keymap_rp/13/009.png" },
	{ 0x16E9, "keymap_rp/13/010.png" },
	{ 0x16F0, "keymap_rp/14/001.png" },
	{ 0x16F1, "keymap_rp/14/002.png" },
	{ 0x16F2, "keymap_rp/14/003.png" },
	{ 0x16F3, "keymap_rp/14/004.png" },
	{ 0x16F4, "keymap_rp/14/005.png" },
	{ 0x16F5, "keymap_rp/14/006.png" },
	{ 0x16F6, "keymap_rp/14/007.png" },
	{ 0x16F7, "keymap_rp/14/008.png" },
	{ 0x16F8, "keymap_rp/14/009.png" },
	{ 0x16F9, "keymap_rp/14/010.png" },
	{ 0x1700, "keymap_rp/15/001.png" },
	{ 0x1701, "keymap_rp/15/002.png" },
	{ 0x1702, "keymap_rp/15/003.png" },
	{ 0x1703, "keymap_rp/15/004.png" },
	{ 0x1704, "keymap_rp/15/005.png" },
	{ 0x1705, "keymap_rp/15/006.png" },
	{ 0x1706, "keymap_rp/15/007.png" },
	{ 0x1707, "keymap_rp/15/008.png" },
	{ 0x1708, "keymap_rp/15/009.png" },
	{ 0x1709, "keymap_rp/15/010.png" },
	{ 0x1710, "keymap_rp/16/001.png" },
	{ 0x1711, "keymap_rp/16/002.png" },
	{ 0x1712, "keymap_rp/16/003.png" },
	{ 0x1713, "keymap_rp/16/004.png" },
	{ 0x1714, "keymap_rp/16/005.png" },
	{ 0x1715, "keymap_rp/16/006.png" },
	{ 0x1716, "keymap_rp/16/007.png" },
	{ 0x1717, "keymap_rp/16/008.png" },
	{ 0x1718, "keymap_rp/16/009.png" },
	{ 0x1719, "keymap_rp/16/010.png" },
	{ 0x1720, "keymap_rp/17/001.png" },
	{ 0x1721, "keymap_rp/17/002.png" },
	{ 0x1722, "keymap_rp/17/003.png" },
	{ 0x1723, "keymap_rp/17/004.png" },
	{ 0x1724, "keymap_rp/17/005.png" },
	{ 0x1725, "keymap_rp/17/006.png" },
	{ 0x1726, "keymap_rp/17/007.png" },
	{ 0x1727, "keymap_rp/17/008.png" },
	{ 0x1728, "keymap_rp/17/009.png" },
	{ 0x1729, "keymap_rp/17/010.png" },
	{ 0x1730, "keymap_rp/18/001.png" },
	{ 0x1731, "keymap_rp/18/002.png" },
	{ 0x1732, "keymap_rp/18/003.png" },
	{ 0x1733, "keymap_rp/18/004.png" },
	{ 0x1734, "keymap_rp/18/005.png" },
	{ 0x1735, "keymap_rp/18/006.png" },
	{ 0x1736, "keymap_rp/18/007.png" },
	{ 0x1737, "keymap_rp/18/008.png" },
	{ 0x1738, "keymap_rp/18/009.png" },
	{ 0x1739, "keymap_rp/18/010.png" },
	{ 0x1740, "keymap_rp/19/001.png" },
	{ 0x1741, "keymap_rp/19/002.png" },
	{ 0x1742, "keymap_rp/19/003.png" },
	{ 0x1743, "keymap_rp/19/004.png" },
	{ 0x1744, "keymap_rp/19/005.png" },
	{ 0x1745, "keymap_rp/19/006.png" },
	{ 0x1746, "keymap_rp/19/007.png" },
	{ 0x1747, "keymap_rp/19/008.png" },
	{ 0x1748, "keymap_rp/19/009.png" },
	{ 0x1749, "keymap_rp/19/010.png" },
	{ 0x1750, "keymap_rp/20/001.png" },
	{ 0x1751, "keymap_rp/20/002.png" },
	{ 0x1752, "keymap_rp/20/003.png" },
	{ 0x1753, "keymap_rp/20/004.png" },
	{ 0x1754, "keymap_rp/20/005.png" },
	{ 0x1755, "keymap_rp/20/006.png" },
	{ 0x1756, "keymap_rp/20/007.png" },
	{ 0x1757, "keymap_rp/20/008.png" },
	{ 0x1758, "keymap_rp/20/009.png" },
	{ 0x1759, "keymap_rp/20/010.png" },
	{ 0x1760, "keymap_rp/21/001.png" },
	{ 0x1761, "keymap_rp/21/002.png" },
	{ 0x1762, "keymap_rp/21/003.png" },
	{ 0x1763, "keymap_rp/21/004.png" },
	{ 0x1764, "keymap_rp/21/005.png" },
	{ 0x1765, "keymap_rp/21/006.png" },
	{ 0x1766, "keymap_rp/21/007.png" },
	{ 0x1767, "keymap_rp/21/008.png" },
	{ 0x1768, "keymap_rp/21/009.png" },
	{ 0x1769, "keymap_rp/21/010.png" },
	{ 0x1770, "keymap_rp/22/001.png" },
	{ 0x1771, "keymap_rp/22/002.png" },
	{ 0x1772, "keymap_rp/22/003.png" },
	{ 0x1773, "keymap_rp/22/004.png" },
	{ 0x1774, "keymap_rp/22/005.png" },
	{ 0x1775, "keymap_rp/22/006.png" },
	{ 0x1776, "keymap_rp/22/007.png" },
	{ 0x1777, "keymap_rp/22/008.png" },
	{ 0x1778, "keymap_rp/22/009.png" },
	{ 0x1779, "keymap_rp/22/010.png" },
	{ 0x1780, "keymap_rp/23/001.png" },
	{ 0x1781, "keymap_rp/23/002.png" },
	{ 0x1782, "keymap_rp/23/003.png" },
	{ 0x1783, "keymap_rp/23/004.png" },
	{ 0x1784, "keymap_rp/23/005.png" },
	{ 0x1785, "keymap_rp/23/006.png" },
	{ 0x1786, "keymap_rp/23/007.png" },
	{ 0x1787, "keymap_rp/23/008.png" },
	{ 0x1788, "keymap_rp/23/009.png" },
	{ 0x1789, "keymap_rp/23/010.png" },
	{ 0x1790, "keymap_rp/24/001.png" },
	{ 0x1791, "keymap_rp/24/002.png" },
	{ 0x1792, "keymap_rp/24/003.png" },
	{ 0x1793, "keymap_rp/24/004.png" },
	{ 0x1794, "keymap_rp/24/005.png" },
	{ 0x1795, "keymap_rp/24/006.png" },
	{ 0x1796, "keymap_rp/24/007.png" },
	{ 0x1797, "keymap_rp/24/008.png" },
	{ 0x1798, "keymap_rp/24/009.png" },
	{ 0x1799, "keymap_rp/24/010.png" },
	{ 0x17A0, "keymap_rp/25/001.png" },
	{ 0x17A1, "keymap_rp/25/002.png" },
	{ 0x17A2, "keymap_rp/25/003.png" },
	{ 0x17A3, "keymap_rp/25/004.png" },
	{ 0x17A4, "keymap_rp/25/005.png" },
	{ 0x17A5, "keymap_rp/25/006.png" },
	{ 0x17A6, "keymap_rp/25/007.png" },
	{ 0x17A7, "keymap_rp/25/008.png" },
	{ 0x17A8, "keymap_rp/25/009.png" },
	{ 0x17A9, "keymap_rp/25/010.png" },
	{ 0x17B0, "keymap_rp/26/001.png" },
	{ 0x17B1, "keymap_rp/26/002.png" },
	{ 0x17B2, "keymap_rp/26/003.png" },
	{ 0x17B3, "keymap_rp/26/004.png" },
	{ 0x17B4, "keymap_rp/26/005.png" },
	{ 0x17B5, "keymap_rp/26/006.png" },
	{ 0x17B6, "keymap_rp/26/007.png" },
	{ 0x17B7, "keymap_rp/26/008.png" },
	{ 0x17B8, "keymap_rp/26/009.png" },
	{ 0x17B9, "keymap_rp/26/010.png" },
	{ 0x17C0, "keymap_rp/27/001.png" },
	{ 0x17C1, "keymap_rp/27/002.png" },
	{ 0x17C2, "keymap_rp/27/003.png" },
	{ 0x17C3, "keymap_rp/27/004.png" },
	{ 0x17C4, "keymap_rp/27/005.png" },
	{ 0x17C5, "keymap_rp/27/006.png" },
	{ 0x17C6, "keymap_rp/27/007.png" },
	{ 0x17C7, "keymap_rp/27/008.png" },
	{ 0x17C8, "keymap_rp/27/009.png" },
	{ 0x17C9, "keymap_rp/27/010.png" },
	{ 0x17D0, "keymap_rp/28/001.png" },
	{ 0x17D1, "keymap_rp/28/002.png" },
	{ 0x17D2, "keymap_rp/28/003.png" },
	{ 0x17D3, "keymap_rp/28/004.png" },
	{ 0x17D4, "keymap_rp/28/005.png" },
	{ 0x17D5, "keymap_rp/28/006.png" },
	{ 0x17D6, "keymap_rp/28/007.png" },
	{ 0x17D7, "keymap_rp/28/008.png" },
	{ 0x17D8, "keymap_rp/28/009.png" },
	{ 0x17D9, "keymap_rp/28/010.png" },
	{ 0x17E0, "keymap_rp/29/001.png" },
	{ 0x17E1, "keymap_rp/29/002.png" },
	{ 0x17E2, "keymap_rp/29/003.png" },
	{ 0x17E3, "keymap_rp/29/004.png" },
	{ 0x17E4, "keymap_rp/29/005.png" },
	{ 0x17E5, "keymap_rp/29/006.png" },
	{ 0x17E6, "keymap_rp/29/007.png" },
	{ 0x17E7, "keymap_rp/29/008.png" },
	{ 0x17E8, "keymap_rp/29/009.png" },
	{ 0x17E9, "keymap_rp/29/010.png" },
	{ 0x17F0, "keymap_rp/30/001.png" },
	{ 0x17F1, "keymap_rp/30/002.png" },
	{ 0x17F2, "keymap_rp/30/003.png" },
	{ 0x17F3, "keymap_rp/30/004.png" },
	{ 0x17F4, "keymap_rp/30/005.png" },
	{ 0x17F5, "keymap_rp/30/006.png" },
	{ 0x17F6, "keymap_rp/30/007.png" },
	{ 0x17F7, "keymap_rp/30/008.png" },
	{ 0x17F8, "keymap_rp/30/009.png" },
	{ 0x17F9, "keymap_rp/30/010.png" },
  };
  char *entry_name = NULL;
  size_t i;
  for (i = 0; i < sizeof entries / sizeof entries[0]; i++)
  {
    if (type == entries[i].type)
    {
      entry_name = entries[i].name;
      break;
    }
  }

  return entry_name;
}

int unpkg(char *pkgfn, char *tidpath)
{
  FILE *in = NULL;
  FILE *out = NULL;
  struct cnt_pkg_main_header m_header;
  struct cnt_pkg_content_header c_header;
  memset(&m_header, 0, sizeof(struct cnt_pkg_main_header));
  memset(&c_header, 0, sizeof(struct cnt_pkg_content_header));

  if ((in = fopen(pkgfn, "rb")) == NULL)
  {
    printfsocket("File not found!\n");
    return 1;
  }

  // Read in the main CNT header (size seems to be 0x180 with 4 hashes included).
  fseek(in, 0, SEEK_SET);
  fread(&m_header, 1,  0x180, in);

  if (m_header.magic != PS4_PKG_MAGIC)
  {
    printfsocket("Invalid PS4 PKG file!\n");
    return 2;
  }

  printfsocket("PS4 PKG header:\n");
  printfsocket("- PKG magic: 0x%X\n", bswap_32(m_header.magic));
  printfsocket("- PKG type: 0x%X\n", bswap_32(m_header.type));
  printfsocket("- PKG table entries: %d\n", bswap_16(m_header.table_entries_num));
  printfsocket("- PKG system entries: %d\n", bswap_16(m_header.system_entries_num));
  printfsocket("- PKG table offset: 0x%X\n", bswap_32(m_header.file_table_offset));
  printfsocket("\n");

  // Seek to offset 0x400 and read content associated header (size seems to be 0x80 with 2 hashes included).
  fseek(in, 0x400, SEEK_SET);
  fread(&c_header, 1,  0x80, in);

  printfsocket("PS4 PKG content header:\n");
  printfsocket("- PKG content offset: 0x%X\n", bswap_32(c_header.content_offset));
  printfsocket("- PKG content size: 0x%X\n", bswap_32(c_header.content_size));
  printfsocket("\n");

  // Locate the entry table and list each type of section inside the PKG/CNT file.
  fseek(in, bswap_32(m_header.file_table_offset), SEEK_SET);

  printfsocket("PS4 PKG table entries:\n");
  struct cnt_pkg_table_entry *entries = malloc(sizeof(struct cnt_pkg_table_entry) * bswap_16(m_header.table_entries_num));
  memset(entries, 0, sizeof(struct cnt_pkg_table_entry) * bswap_16(m_header.table_entries_num));
  int i;
  for (i = 0; i < bswap_16(m_header.table_entries_num); i++)
  {
    fread(&entries[i], 1,  0x20, in);
    printfsocket("Entry #%d\n", i);
    printfsocket("- PKG table entry type: 0x%X\n", bswap_32(entries[i].type));
    printfsocket("- PKG table entry offset: 0x%X\n", bswap_32(entries[i].offset));
    printfsocket("- PKG table entry size: 0x%X\n", bswap_32(entries[i].size));
    printfsocket("\n");
  }

  // Vars for file name listing.
  struct file_entry *entry_files = malloc(sizeof(struct file_entry) * bswap_16(m_header.table_entries_num));
  memset(entry_files, 0, sizeof(struct file_entry) * bswap_16(m_header.table_entries_num));
  char *file_name_list[256];
  int file_name_index = 0;
  int file_count = 0;

  // Var for file writing.
  unsigned char *entry_file_data;

  // Search through the data entries and locate the name table entry.
  // This section should keep relevant strings for internal files inside the PKG/CNT file.
  for (i = 0; i < bswap_16(m_header.table_entries_num); i++)
  {
    if (bswap_32(entries[i].type) == PS4_PKG_ENTRY_TYPE_NAME_TABLE)
    {
      printfsocket("Found name table entry. Extracting file names:\n");
      fseek(in, bswap_32(entries[i].offset) + 1, SEEK_SET);
      while ((file_name_list[file_name_index] = read_string(in))[0] != '\0')
      {
        printfsocket("%s\n", file_name_list[file_name_index]);
        file_name_index++;
      }
      printfsocket("\n");
    }
  }

  // Search through the data entries and locate file entries.
  // These entries need to be mapped with the names collected from the name table.
  for (i = 0; i < bswap_16(m_header.table_entries_num); i++)
  {
    // Use a predefined list for most file names.
    entry_files[i].name = get_entry_name_by_type(bswap_32(entries[i].type));
    entry_files[i].offset = bswap_32(entries[i].offset);
    entry_files[i].size = bswap_32(entries[i].size);

    if (((bswap_32(entries[i].type) & PS4_PKG_ENTRY_TYPE_FILE1) == PS4_PKG_ENTRY_TYPE_FILE1)
    || (((bswap_32(entries[i].type) & PS4_PKG_ENTRY_TYPE_FILE2) == PS4_PKG_ENTRY_TYPE_FILE2)))
    {
      // If a file was found and it's name is not on the predefined list, try to map it with
      // a name from the name table.
      if (entry_files[i].name == NULL)
      {
        entry_files[i].name = file_name_list[file_count];
      }
      if (entry_files[i].name != NULL)
      {
        file_count++;
      }
    }
  }
  printfsocket("Successfully mapped %d files.\n\n", file_count);

  // Set up the output directory for file writing.
  char dest_path[256];
  char title_id[256];

  memset(title_id, 0, 256);
  memcpy(title_id, tidpath, 255);
  mkdir(title_id, 0777);

  // Search through the entries for mapped file data and output it.
  printfsocket("Dumping internal PKG files...\n");
  for (i = 0; i < bswap_16(m_header.table_entries_num); i++)
  {
    entry_file_data = (unsigned char *)realloc(NULL, entry_files[i].size);

    fseek(in, entry_files[i].offset, SEEK_SET);
    fread(entry_file_data, 1,  entry_files[i].size, in);

    if (entry_files[i].name == NULL) continue;

    sprintf(dest_path, "%s/sce_sys/%s", title_id, entry_files[i].name);
    printfsocket("%s\n", dest_path);

    _mkdir (dest_path);

    if ((out = fopen(dest_path, "wb")) == NULL )
    {
      printfsocket("Can't open file for writing!\n");
      return 3;
    }

    fwrite(entry_file_data, 1, entry_files[i].size, out);

    fclose(out);
  }

  // Clean up.
  fclose(in);

  free(entries);
  free(entry_files);

  printfsocket("Done.\n");

  return 0;
}
