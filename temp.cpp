
if (onelineback.size())
{
    stringstream temp(str);
    temp >> tempstr;
    while (temp >> tempstr)
        if (onelineback.find(tempstr) != onelineback.end())
        {
            mem.push_back("nop");
            break;
        }
    stringstream temp(str);
    temp >> tempstr;
}