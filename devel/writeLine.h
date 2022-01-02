sbddextended_INLINE_FUNC
int sbddextended_write_inner(const char* buf, FILE* fp)
{
    if (fputs(buf, fp) == EOF) {
        return 0;
    }
    return 1;
}

sbddextended_INLINE_FUNC
int sbddextended_writeLine_inner(const char* buf, FILE* fp)
{
    if (fputs(buf, fp) == EOF || fputc('\n', fp) == EOF) {
        return 0;
    }
    return 1;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint8_inner(unsigned char v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned char), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint16_inner(unsigned short v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned short), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint32_inner(unsigned int v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned int), 1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint64_inner(ullint v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(ullint), 1, fp) != 0;
}

#ifdef __cplusplus

class WriteObject {
private:
    const bool is_fstream_;
    const bool is_ln_;
    std::ostream* ost_;

public:
    WriteObject(bool is_fstream, bool is_ln, std::ostream* ost)
        : is_fstream_(is_fstream), is_ln_(is_ln), ost_(ost) {}

    bool operator()(const char* buf, FILE* fp) const {
        if (is_fstream_) {
            if (!*ost_ || !(*ost_ << buf)) {
                return false;
            }
            if (is_ln_) {
                if (!*ost_ || !(*ost_ << '\n')) {
                    return false;
                }
            }
        } else {
            assert(fp != NULL);
            if (is_ln_) {
                return sbddextended_writeLine_inner(buf, fp) != 0;
            } else {
                return sbddextended_write_inner(buf, fp) != 0;
            }
        }
        return true;
    }

    bool operator()(unsigned char v, FILE* fp) const {
        //std::cerr << "uint8 " << (ullint)v << std::endl;
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(unsigned char));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint8_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(unsigned short v, FILE* fp) const {
        //std::cerr << "uint16 " << (ullint)v << std::endl;
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(unsigned short));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint16_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(unsigned int v, FILE* fp) const {
        //std::cerr << "uint32 " << (ullint)v << std::endl;
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(unsigned int));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint32_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(ullint v, FILE* fp) const {
        //std::cerr << "uint64 " << (ullint)v << std::endl;
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            ost_->write(reinterpret_cast<char*>(&v), sizeof(ullint));
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint64_inner(v, fp) != 0;
        }
        return true;
    }
};

#else

sbddextended_INLINE_FUNC
int sbddextended_write(const char* buf, FILE* fp)
{
    return sbddextended_write_inner(buf, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeLine(const char* buf, FILE* fp)
{
    return sbddextended_writeLine_inner(buf, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint8(unsigned char v, FILE* fp)
{
    return sbddextended_writeUint8_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint16(unsigned short v, FILE* fp)
{
    return sbddextended_writeUint16_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint32(unsigned int v, FILE* fp)
{
    return sbddextended_writeUint32_inner(v, fp);
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint64(ullint v, FILE* fp)
{
    return sbddextended_writeUint64_inner(v, fp);
}


#endif
