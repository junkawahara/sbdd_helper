sbddextended_INLINE_FUNC
int sbddextended_readChar_inner(FILE* fp)
{
    return fgetc(fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readLine_inner(char* buf, FILE* fp)
{
    size_t n;
    char* p;
    p = fgets(buf, sbddextended_MAX_LINE, fp);
    if (p == NULL) {
        return 0;
    }
    n = strlen(buf);
    if (buf[n - 1] != '\n') {
        fprintf(stderr, "Each line must not exceed %d characters\n",
                sbddextended_MAX_LINE);
        exit(1);
    }
    return 1;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint8_inner(unsigned char* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned char), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint16_inner(unsigned short* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned short), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint32_inner(unsigned int* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(unsigned int), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_readUint64_inner(ullint* v, FILE* fp)
{
    assert(fp != NULL);
    return fread(v, sizeof(ullint), 1, fp) != 0;
}


#ifdef __cplusplus

class ReadCharObject {
protected:
    static const int STREAM = 0;
    static const int FP = 1;
    static const int STRING = 2;
    typedef int Mode;

    Mode mode_;
    std::istream* ist_;
    const char* const st_;
    llint stpos_;
    const llint stlen_;

public:
    ReadCharObject()
        : mode_(FP), ist_(NULL), st_(NULL), stpos_(0), stlen_(0) { }

    ReadCharObject(std::istream* ist)
        : mode_(STREAM), ist_(ist), st_(NULL), stpos_(0), stlen_(0) { }

    ReadCharObject(const std::string& st)
        : mode_(STRING), ist_(NULL), st_(st.c_str()), stpos_(0),
          stlen_(static_cast<llint>(st.length())) { }

    ReadCharObject(const char* st)
        : mode_(STRING), ist_(NULL), st_(st), stpos_(0),
          stlen_((st_ != NULL) ? strlen(st_) : 0) { }

    int operator()(FILE* fp) {
        switch (mode_) {
        case STREAM:
            return ist_->get();
        case FP:
            return fgetc(fp);
        case STRING:
            if (stpos_ >= stlen_) {
                return -1;
            } else {
                ++stpos_;
                return st_[stpos_ - 1];
            }
        }
        return -1; // never come here
    }

    bool operator()(unsigned char* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(unsigned char));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint8_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }

    bool operator()(unsigned short* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(unsigned short));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint16_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }

    bool operator()(unsigned int* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(unsigned int));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint32_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }

    bool operator()(ullint* v, FILE* fp) const {
        switch (mode_) {
        case STREAM:
            ist_->read(reinterpret_cast<char*>(v), sizeof(ullint));
            if (!ist_ || ist_->eof()) {
                return false;
            }
            break;
        case FP:
            return sbddextended_readUint64_inner(v, fp) != 0;
        case STRING:
            std::cerr << "not implemented" << std::endl;
            return false;
        }
        return true;
    }
};

class ReadLineObject : public ReadCharObject {
public:
    ReadLineObject()
        : ReadCharObject() { }

    ReadLineObject(std::istream* ist)
        : ReadCharObject(ist) { }

    ReadLineObject(const std::string& st)
        : ReadCharObject(st) { }

    ReadLineObject(const char* st)
        : ReadCharObject(st) { }

    bool operator()(char* buf, FILE* fp) {
        std::string str;
        switch (mode_) {
        case STREAM:
            if (!std::getline(*ist_, str)) {
                return false;
            }
            if (str.length() >= sbddextended_MAX_LINE - 1) {
                fprintf(stderr, "Each line must not exceed %d characters\n",
                    sbddextended_MAX_LINE);
                exit(1);
            }
            strcpy(buf, str.c_str());
            return true;
        case FP:
            return sbddextended_readLine_inner(buf, fp) != 0;
        case STRING:
            if (stpos_ >= stlen_) {
                return false;
            } else {
                llint start = stpos_;
                while (stpos_ < stlen_ && st_[stpos_] != '\n') {
                    ++stpos_;
                    if (stpos_ - start >= sbddextended_MAX_LINE - 1) {
                        fprintf(stderr, "Each line must not exceed %d characters\n",
                                sbddextended_MAX_LINE);
                        exit(1);
                    }
                }
                strncpy(buf, st_ + start, stpos_ - start);
                ++stpos_;
                return true;
            }
        }
        return false; // never come here
    }
};

#else

sbddextended_INLINE_FUNC
int sbddextended_readChar(FILE* fp)
{
    return sbddextended_readChar_inner(fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readLine(char* buf, FILE* fp)
{
    return sbddextended_readLine_inner(buf, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint8(unsigned char* v, FILE* fp)
{
    return sbddextended_readUint8_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint16(unsigned short* v, FILE* fp)
{
    return sbddextended_readUint16_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint32(unsigned int* v, FILE* fp)
{
    return sbddextended_readUint32_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_readUint64(ullint* v, FILE* fp)
{
    return sbddextended_readUint64_inner(v, fp);
}



#endif
