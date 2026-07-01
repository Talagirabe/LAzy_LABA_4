#ifndef LABA4_SEQUENCE_STREAMS_H
#define LABA4_SEQUENCE_STREAMS_H

#include <fstream>
#include <string>
#include <stdexcept>
#include "../../LABA_2/sequence.h"

// 1. БАЗОВЫЕ ИНТЕРФЕЙС

template<class T>
class IStream {
public:
    virtual ~IStream() = default;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual bool is_open() const = 0;
    virtual int get_position() const = 0;
};

template<class T>
class InputStream : public IStream<T> {
public:
    virtual bool is_end_of_stream() const = 0;
    virtual bool can_seek() const = 0;
    virtual bool can_go_back() const = 0;
    virtual void seek(int pos) = 0;
    virtual const T& read() = 0;
};

template<class T>
class OutputStream : public IStream<T> {
public:
    virtual void write(const T& item) = 0;
};

// 2. ФАЙЛОВЫЕ ПОТОКИ (FileInputStream / FileOutputStream)

class FileInputStream : public InputStream<char> {
private:
    std::ifstream file;
    std::string path;
    bool opened;
    int position;
    char current_char;//как метод read() обязан возвращать результат по константной ссылке
                      //считываем символ из файла в эту переменную, а затем возвращаем ссылку на нее.
public:
    FileInputStream(const std::string& path)
        : path(path), opened(false), position(0), current_char('\0') {}

    ~FileInputStream() override { if (file.is_open()) file.close(); }

    void open() override {
        file.open(path);
        if (!file.is_open()) throw std::runtime_error("Failed to open file");
        opened = true;
        position = 0;
    }

    void close() override { file.close(); opened = false; }
    bool is_open() const override { return opened; }
    bool is_end_of_stream() const override { return file.eof(); } //возвращает true, если мы уперлись в конец файла (End Of File)
    bool can_seek() const override { return true; }
    bool can_go_back() const override { return true; }
    int get_position() const override { return position; }

    void seek(int pos) override {
        if (pos < 0) throw std::out_of_range("Negative seek position");
        file.clear(); // сбрасывает ошибки потока
        file.seekg(pos);
        if (file.fail()) throw std::runtime_error("seek failed");
        position = pos;
    }

    const char& read() override {
        if (!opened) throw std::logic_error("Stream is closed");
        if (!file.get(current_char)) throw std::out_of_range("End of stream reached");
        position++;
        return current_char;
    }
};

class FileOutputStream : public OutputStream<char> {
private:
    std::ofstream file;
    std::string path;
    bool opened;
    int position;

public:
    FileOutputStream(const std::string& path) : path(path), opened(false), position(0) {}
    ~FileOutputStream() override { if (file.is_open()) file.close(); }

    void open() override {
        file.open(path);
        if (!file.is_open()) throw std::runtime_error("Failed to open file");
        opened = true;
        position = 0;
    }

    void close() override { file.close(); opened = false; }
    bool is_open() const override { return opened; }
    int get_position() const override { return position; }

    void write(const char& item) override {
        if (!is_open()) throw std::logic_error("Stream is closed");
        file.put(item);
        if (file.fail()) throw std::runtime_error("Write failed");
        position++;
    }
};

// 3. СТРОКОВЫЕ ПОТОКИ (StringInputStream / StringOutputStream)

class StringInputStream : public InputStream<char> {
private:
    std::string source;
    bool opened;
    int position;
    char current_char;

public:
    StringInputStream(const std::string& source)
        : source(source), opened(false), position(0), current_char('\0') {}

    void open() override { opened = true; position = 0; }
    void close() override { opened = false; }
    bool is_open() const override { return opened; }
    bool is_end_of_stream() const override { return position >= source.size(); }
    bool can_seek() const override { return true; }
    bool can_go_back() const override { return true; }
    int get_position() const override { return position; }

    void seek(int pos) override {
        if (pos < 0) throw std::out_of_range("Negative seek position");
        if (pos > source.size()) throw std::out_of_range("Seek out of range");
        position = pos;
    }

    const char& read() override {
        if (!opened) throw std::logic_error("Stream is closed");
        if (is_end_of_stream()) throw std::out_of_range("End of stream reached");
        current_char = source[position];
        position++;
        return current_char;
    }
};

class StringOutputStream : public OutputStream<char> {
private:
    std::string buffer;
    bool opened;
    int position;

public:
    StringOutputStream() : buffer(""), opened(false), position(0) {}
    void open() override { opened = true; buffer.clear(); position = 0; }
    void close() override { opened = false; }
    bool is_open() const override { return opened; }
    int get_position() const override { return position; }

    void write(const char& item) override {
        if (!opened) throw std::logic_error("Stream is closed");
        buffer += item;
        position++;
    }
    const std::string& get_string() const { return buffer; }
};

// 4. ПОТОКИ ДЛЯ ПОСЛЕДОВАТЕЛЬНОСТЕЙ (Sequence Streams)

template<class T>
class SequenceInputStream : public InputStream<T> {
private:
    Sequence<T>* source;
    int position;
    bool opened;

public:
    SequenceInputStream(Sequence<T>* source) : source(source), position(0), opened(false) {}

    void open() override { opened = true; }
    void close() override { opened = false; }
    bool is_open() const override { return opened; }

    bool is_end_of_stream() const override {
        Cardinal length = source->get_length();
        if (length.is_infinite()) return false;
        return position >= length.get_offset();
    }

    bool can_seek() const override { return true; }
    bool can_go_back() const override { return true; }
    int get_position() const override { return position; }

    void seek(int new_position) override {
        if (new_position < 0) throw std::out_of_range("Negative stream position");
        Cardinal length = source->get_length();
        if (!length.is_infinite() && new_position > length.get_offset())
            throw std::out_of_range("Seek position out of range");
        position = new_position;
    }

    const T& read() override {
        if (!opened) throw std::logic_error("Stream is closed");
        if (is_end_of_stream()) throw std::out_of_range("End of stream reached");
        return source->get(position++);
    }
};

template<class T>
class SequenceOutputStream : public OutputStream<T> {
private:
    Sequence<T>* target;
    int position;
    bool opened;

public:
    SequenceOutputStream(Sequence<T>* target) : target(target), position(0), opened(false) {}

    void open() override { opened = true; }
    void close() override { opened = false; }
    bool is_open() const override { return opened; }
    int get_position() const override { return position; }

    void write(const T& item) override {
        if (!opened) throw std::logic_error("Stream is closed");
        target = target->append(item);
        position++;
    }

    Sequence<T>* get_target() const { return target; }
};

#endif //LABA4_SEQUENCE_STREAMS_H