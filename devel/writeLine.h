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

/* Converts a value into a byte sequence of a fixed width in little endian */
/* so that the produced binary does not depend on the byte order nor the */
/* type sizes of the machine. See also the comment of */
/* sbddextended_bytesToUint16 in readLine.h. */
sbddextended_INLINE_FUNC
void sbddextended_uint16ToBytes(unsigned short v, unsigned char* buf)
{
    int i;
    for (i = 0; i < 2; ++i) {
        buf[i] = (unsigned char)(v & 0xffu);
        v = (unsigned short)(v >> 8);
    }
}

sbddextended_INLINE_FUNC
void sbddextended_uint32ToBytes(unsigned int v, unsigned char* buf)
{
    int i;
    for (i = 0; i < 4; ++i) {
        buf[i] = (unsigned char)(v & 0xffu);
        v >>= 8;
    }
}

sbddextended_INLINE_FUNC
void sbddextended_uint64ToBytes(ullint v, unsigned char* buf)
{
    int i;
    for (i = 0; i < 8; ++i) {
        buf[i] = (unsigned char)(v & 0xffu);
        v >>= 8;
    }
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint8_inner(unsigned char v, FILE* fp)
{
    assert(fp != NULL);
    return fwrite(&v, sizeof(unsigned char), (size_t)1, fp) != 0;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint16_inner(unsigned short v, FILE* fp)
{
    unsigned char buf[2];
    assert(fp != NULL);
    sbddextended_uint16ToBytes(v, buf);
    return fwrite(buf, sizeof(unsigned char), (size_t)2, fp) == (size_t)2;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint32_inner(unsigned int v, FILE* fp)
{
    unsigned char buf[4];
    assert(fp != NULL);
    sbddextended_uint32ToBytes(v, buf);
    return fwrite(buf, sizeof(unsigned char), (size_t)4, fp) == (size_t)4;
}

sbddextended_INLINE_FUNC
int sbddextended_writeUint64_inner(ullint v, FILE* fp)
{
    unsigned char buf[8];
    assert(fp != NULL);
    sbddextended_uint64ToBytes(v, buf);
    return fwrite(buf, sizeof(unsigned char), (size_t)8, fp) == (size_t)8;
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
        /*std::cerr << "uint8 " << (ullint)v << std::endl; */
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            if (!ost_->write(reinterpret_cast<char*>(&v),
                             sizeof(unsigned char))) {
                return false;
            }
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint8_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(unsigned short v, FILE* fp) const {
        /*std::cerr << "uint16 " << (ullint)v << std::endl; */
        unsigned char buf[2];
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            sbddextended_uint16ToBytes(v, buf);
            if (!ost_->write(reinterpret_cast<char*>(buf), 2)) {
                return false;
            }
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint16_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(unsigned int v, FILE* fp) const {
        /*std::cerr << "uint32 " << (ullint)v << std::endl; */
        unsigned char buf[4];
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            sbddextended_uint32ToBytes(v, buf);
            if (!ost_->write(reinterpret_cast<char*>(buf), 4)) {
                return false;
            }
        } else {
            assert(fp != NULL);
            return sbddextended_writeUint32_inner(v, fp) != 0;
        }
        return true;
    }

    bool operator()(ullint v, FILE* fp) const {
        /*std::cerr << "uint64 " << (ullint)v << std::endl; */
        unsigned char buf[8];
        if (is_fstream_) {
            if (!*ost_) {
                return false;
            }
            sbddextended_uint64ToBytes(v, buf);
            if (!ost_->write(reinterpret_cast<char*>(buf), 8)) {
                return false;
            }
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
