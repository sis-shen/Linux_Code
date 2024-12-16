#include <string>

class ColorReplace
{
public:
    enum Color{
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,//品红色
        Cyan,//青色
        White,
        None //无颜色
    };

    static std::string colorReplace(const std::string& str,const std::string&pattern,Color color)
    {
        size_t prev = 0;
        size_t pos = 0;
        std::string ret;
        while(pos <str.size() &&  pos != std::string::npos)
        {
            prev = pos;
            pos = str.find(pattern,pos);
            ret+=str.substr(prev,pos-prev);
            if(pos!= std::string::npos)
            {
                ret+=colorToStr(color);
                ret+=pattern;
                ret+=colorToStr(Color::None);
                pos+=pattern.size();
            }
            else 
            {
                break;
            }
        }

        return ret;
    }

    static inline std::string colorToStr(Color color)
    {
        switch(color)
        {
            case Black:return "\033[30m";break;
            case Red:return "\033[31m";break;
            case Green:return "\033[32m";break;
            case Yellow:return "\033[33m";break;
            case Blue:return "\033[34m";break;
            case Magenta:return "\033[35m";break;
            case Cyan:return "\033[36m";break;
            case White:return "\033[37m";break;
            case Color::None:return "\033[0m";break;

            default: return "\033[0m";
        }
    }
};