#ifndef MESSAGE.H
#define MESSAGE.H

class message{
    private:
        int message_id;
    public:
        int author_id;
        std::string author_name;
        std::string content;

};

#endif /** MESSAGE-H */