
enum ROMSize {
    KB32,
    KB64,
    KB128,
    KB256,
    KB512,
    MB1,
    MB2,
    MB4,
    MB1p1,
    MB1p2,
    MB1p5,
};

enum RAMSize {
    None,
    KB2,
    KB8,
    KB32,
    KB128,
    KB64,
};

enum class Destination {
    Japanese,
    NonJapanese,
};

class Cartridge
{
private:
    /* data */
public:
    Cartridge(/* args */);
    ~Cartridge();
};

Cartridge::Cartridge(/* args */)
{
}

Cartridge::~Cartridge()
{
}
