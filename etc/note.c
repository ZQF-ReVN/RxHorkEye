struct HorkEye_CRC64
{
	uint32_t uiHash_L;
	uint32_t uiHash_H;
};

struct HorkEye_STD_String
{
  union
  {
    char aStr[16];
    char* pStr;
  };
	uint32_t uiLen;
	uint32_t uiReserve;
}

struct HorkEye_Map_Node
{
  uint32_t uiUn0;
  HorkEye_Entry_Node Entry_Node;
  uint32_t uiUn1;
};

struct HorkEye_Entry_Node
{
  HorkEye_Entry_Node *uiNextNode;
  uint32_t uiUn1;
  HorkEye_Entry Entry;
};

struct HorkEye_Entry
{
  HorkEye_CRC64 Hash;
  HorkEye_Entry_Info Info;
};

struct HorkEye_Entry_Info
{
  uint8_t aPackPath[256];
  uint32_t uiType; // 1 == tlg,ogg; 0 === avi; 5 == nut
  uint32_t uiFOA;
  uint32_t uiUn2;
  uint32_t uiUn3;
};
