```c++
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
};

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

```





## HorkEye::VFS::Media::Read

```c
bool __cdecl HorkEye__VFS__Media__Read(void *cpFilePath, uint32_t **ppBuffer, size_t *pSize_Ret, uint32_t *pBufferMaxSize)
```

概述：读取目标文件（主要是图片tlg和音频ogg），得到大小，文件buffer，需要的时候调整buffer大小

参数一：文件路径字符串

参数二：`uint32_t`指针，指向buffer指针的指针

参数三：`uint32_t`指针，用于返回文件的大小

参数四：`uint32_t`指针，指向buffer的大小

流程概述：

​	先调用 ·`HorkEye::VFS::OpenFile` 来获得文件指针和文件大小以及文件路径的hash值，然后在 `sg_umpPackIndex` 中查找`HorkEye_Entry`并判断是否需要解压（期间会检查buffer大小和指针，以此来确定是否需要重新分配空间）

​	需要解压，分配空间，fread，则用ZLIB进行解压，然后设置`pSize_Ret`，并返回true

​	不需要解压，则fread，并调用 `HorkEye::VFS::Data::Decode` 进行解密，并返回true



## HorkEye::VFS::Nut::Read

```C
bool __cdecl HorkEye__VFS__Nut__Read(const char *cpFilePath, HorkEye_STD_String *pScriptStr)
```

概述：读取`nut`脚本,即SQuirrel脚本

参数一：文件路径字符串

参数二：`HorkEye_STD_String`指针，指向`std::string`用于存储`nut`脚本字符串

流程概述：

​	调用 ·`HorkEye::VFS::OpenFile` 来获得文件指针和文件大小以及文件路径的hash值，构造std::string，fread数据进std::string，返回



## HorkEye::VFS::OpenFile


```c
FILE *__cdecl HorkEye__VFS__OpenFile(const char *cpFilePath, uint32_t *pSize_Ret, HorkEye_CRC64 *pFilePathHash_Ret)
```

概述：得到目标的文件的FStream，文件大小，Hash值（仅仅封包读取）

参数一：文件路径字符串

参数二：`uint32_t`指针，用于输出文件的大小

参数三：`HorkEye_CRC64`指针，用于返回计算的`crc64`值

流程概述：

​	先把输入的文件路径计算`crc64`，也就是文件路径的`hash`值

​	测试全局变量 `sg_uiReadMode` 如果为1则先尝试从封包读取，如果为0则从游戏目录下读取

​	如果从封包读取：先在`sg_umpPackIndex`里查找`hash`值是否存在，

​		如果存在则读取`HorkEye_Entry`，并用`fopen`打开对应封包，`fseek`到目标文件的偏移，设置`pSize_Ret`，设置`pFilePathHash_Ret`，然后返回文件指针

​		如果不存在，则跳到目录读取

​	如果从目录读取：直接在游戏目录下打开输入路径的文件，如果成功，则获取大小，设置`pSize_Ret`，设置`pFilePathHash_Ret`为0，并返回文件指针



## HorkEye::VFS::Data::Decode

```C
HorkEye_Entry *__cdecl HorkEye__VFS__Data__Decode(const char *pPath, uint32_t *pSize1, HorkEye_CRC64 *pHash, uint32_t **pBuffer, uint32_t *pSize2)
```

概述：解密封包数据

参数一：文件路径字符串

参数二：`uint32_t`指针，指向文件的大小

参数三：`HorkEye_CRC64`指针，指向由路径计算的`crc64`值（`uint64_t`大小）

参数四：`uint32_t`指针，指向文件`buffer`指针

参数五：`uint32_t`指针，指向`buffer`大小（通常和文件一样大）

流程概述：

​	如果pHash为nullptr则直接返回（适用于目录读取）

​	先通过hash值来查找 `sg_umpPackIndex` 并得到 `HorkEye_Entry` 的指针，然后获取类型，如果类型不是0，则对`buffer`的数据进行解密，否则返回，返回值一般无用
