struct ACV_Hash
{
	uint32_t uiHash_L;
	uint32_t uiHash_H;
};

struct ACV_STD_String
{
	 union
  {
    char aStr[16];
    char* pStr;
  };
	uint32_t uiLen;
	uint32_t uiReserve;
}

struct ACV_Map_Node
{
  uint32_t uiUn0;
  ACV_Entry_Node Entry_Node;
  uint32_t uiUn1;
};

struct ACV_Entry_Node
{
  ACV_Entry_Node *uiNextNode;
  uint32_t uiUn1;
  ACV_Entry Entry;
};

struct ACV_Entry
{
  ACV_Hash Hash;
  ACV_Entry_Info Info;
};

struct ACV_Entry_Info
{
  uint8_t aPackPath[256];
  uint32_t uiType; // 1 == tlg,ogg; 0 === avi; 5 == nut
  uint32_t uiFOA;
  uint32_t uiUn2;
  uint32_t uiUn3;
};
