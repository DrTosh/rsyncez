#include <string>
#include <vector>

std::vector<std::string> splitString(std::string _text, char _delimiter, bool saveQuoted)
{
    std::vector<std::string> result;
    std::string current;
    bool inQuote = false;

    for(int run = 0; run < _text.length(); run++)
    {
        if(_text[run] == '"')
        {
            inQuote = !inQuote;
        }

        if(_text[run] == _delimiter && (!saveQuoted || !inQuote))
        {
            if(current != "")
            {
                result.push_back(current);
            }            
            current = "";
        }
        else
        {
            current += _text[run];
        }
    }

    if(current != "")
    {
        result.push_back(current);
    }   

    return result;
}