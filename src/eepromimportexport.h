/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 *
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef eeprom_importexport_h
#define eeprom_importexport_h

#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))

class DataField {
  public:
    virtual ~DataField() { }
    virtual void ExportBits(QBitArray & output) = 0;
    virtual void ImportBits(QBitArray & input) = 0;
    virtual unsigned int size() = 0;

    QBitArray bytesToBits(QByteArray bytes)
    {
      QBitArray bits(bytes.count()*8);
      // Convert from QByteArray to QBitArray
      for (int i=0; i<bytes.count(); ++i)
        for (int b=0; b<8; ++b)
          bits.setBit(i*8+b, bytes.at(i)&(1<<b));
      return bits;
    }

    QByteArray bitsToBytes(QBitArray bits)
    {
      QByteArray bytes;
      bytes.resize((7+bits.count())/8);
      bytes.fill(0);
      // Convert from QBitArray to QByteArray
      for (int b=0; b<bits.count(); ++b)
        bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(b%8)));
      return bytes;
    }

    int Export(QByteArray & output)
    {
      QBitArray result;
      ExportBits(result);
      output = bitsToBytes(result);
      return 0;
    }

    int Import(QByteArray & input)
    {
      QBitArray bits = bytesToBits(input);
      DataField::ImportBits(bits);
      return 0;
    }

    virtual void Dump(const char *title)
    {
      QByteArray tmp;
      Export(tmp);
      printf("%s\n", title);
      foreach(char c, tmp) {
        printf("%d, ", (int)c);
      }
      printf("\n\n"); fflush(stdout);
    }
};

template<int N>
class UnsignedField: public DataField {
  public:
    UnsignedField(unsigned int & field):
      field(field)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      output.resize(N);
      for (int i=0; i<N; i++) {
        if (field & (1<<i))
          output.setBit(i);
      }
    }

    virtual void ImportBits(QBitArray & input)
    {
      field = 0;
      for (int i=0; i<N; i++) {
        if (input[i])
          field |= (1<<i);
      }
    }

    virtual unsigned int size()
    {
      return N;
    }

  protected:
    unsigned int & field;
};

template<int N>
class SignedField: public UnsignedField<N> {
  public:
    SignedField(int & field):
      UnsignedField<N>((unsigned int &)field)
    {
    }

    virtual void ImportBits(QBitArray & input)
    {
      UnsignedField<N>::ImportBits(input);
      if (input[N-1]) {
        for (unsigned int i=N; i<8*sizeof(int); i++) {
          UnsignedField<N>::field |= (1<<i);
        }
      }
    }
};

template<int N>
class SpareBitsField: public UnsignedField<N> {
  public:
    SpareBitsField():
      UnsignedField<N>(spare),
      spare(0)
    {
    }
  protected:
    unsigned int spare;
};

int8_t char2idx(char c);
char idx2char(int8_t idx);

template<int N>
class ZCharField: public DataField {
  public:
    ZCharField(char *field):
      field(field)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      output.resize(N*8);
      int b = 0;
      for (int i=0; i<N; i++) {
        int idx = char2idx(field[i]);
        for (int j=0; j<8; j++, b++) {
          if (idx & (1<<j))
            output.setBit(b);
        }
      }
    }

    virtual void ImportBits(QBitArray & input)
    {
      unsigned int b = 0;
      for (int i=0; i<N; i++) {
        int8_t idx = 0;
        for (int j=0; j<8; j++) {
          if (input[b++])
            idx |= (1<<j);
        }
        field[i] = idx2char(idx);
      }

      field[N] = '\0';
      for (int i=N-1; i>=0; i--) {
        if (field[i] == ' ')
          field[i] = '\0';
        else
          break;
      }
    }

    virtual unsigned int size()
    {
      return 8*N;
    }

  protected:
    char * field;
};

class StructField: public DataField {
  public:
    ~StructField() {
      foreach(DataField *field, fields) {
        delete field;
      }
    }

    inline void Append(DataField *field) {
      fields.append(field);
    }

    virtual void ExportBits(QBitArray & output)
    {
      int offset = 0;
      output.resize(size());
      foreach(DataField *field, fields) {
        QBitArray bits;
        field->ExportBits(bits);
        for (int i=0; i<bits.size(); i++)
          output[offset++] = bits[i];
      }
    }

    virtual void ImportBits(QBitArray & input)
    {
      int offset = 0;
      foreach(DataField *field, fields) {
        unsigned int size = field->size();
        QBitArray bits(size);
        for (unsigned int i=0; i<size; i++) {
          bits[i] = input[offset++];
        }
        field->ImportBits(bits);
      }
    }

    virtual unsigned int size()
    {
      unsigned int result = 0;
      foreach(DataField *field, fields) {
        result += field->size();
      }
      return result;
    }

  protected:
    QList<DataField *> fields;
};

class TransformedField: public DataField {
  public:
    TransformedField(DataField & field):
      field(field)
    {
    }

    virtual ~TransformedField()
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      beforeExport();
      field.ExportBits(output);
    }

    virtual void ImportBits(QBitArray & input)
    {
      field.ImportBits(input);
      afterImport();
    }

    virtual unsigned int size()
    {
      return field.size();
    }

    virtual void beforeExport() = 0;

    virtual void afterImport() = 0;

  protected:
    DataField & field;
};

template<int N>
class BoolField: public TransformedField {
  public:
    BoolField(bool & b):
      TransformedField(internalField),
      internalField((unsigned int &)_b),
      b(b),
      _b(0)
    {
    }

    virtual void beforeExport()
    {
      _b = b;
    }

    virtual void afterImport()
    {
      b = _b;
    }

  protected:
    UnsignedField<N> internalField;
    bool & b;
    bool _b;
};

#define TABLE_CONVERSION(table) DIM(table)/2, table

template<class T>
class ConversionField: public TransformedField {
  public:
    ConversionField(int & field, const unsigned int count, const int *table, const QString & error = ""):
      TransformedField(internalField),
      internalField(_field),
      field(field),
      _field(0),
      count(count),
      table(table),
      shift(0),
      exportFunc(NULL),
      importFunc(NULL),
      error(error)
    {
    }

    ConversionField(unsigned int & field, const unsigned int count, const int *table, const QString & error = ""):
      TransformedField(internalField),
      internalField((unsigned int &)_field),
      field((int &)field),
      _field(0),
      count(count),
      table(table),
      shift(0),
      exportFunc(NULL),
      importFunc(NULL),
      error(error)
    {
    }

    ConversionField(int & field, int (*exportFunc)(int), int (*importFunc)(int)):
      TransformedField(internalField),
      internalField(_field),
      field(field),
      _field(0),
      count(0),
      table(NULL),
      shift(0),
      exportFunc(exportFunc),
      importFunc(importFunc),
      error("")
    {
    }

    ConversionField(int & field, int shift):
      TransformedField(internalField),
      internalField(_field),
      field(field),
      _field(0),
      count(0),
      table(NULL),
      shift(shift),
      exportFunc(NULL),
      importFunc(NULL),
      error("")
    {
    }

    ConversionField(unsigned int & field, int shift):
      TransformedField(internalField),
      internalField((unsigned int &)_field),
      field((int &)field),
      _field(0),
      count(0),
      table(NULL),
      shift(shift),
      exportFunc(NULL),
      importFunc(NULL),
      error("")
    {
    }

    virtual void beforeExport()
    {
      if (table) {
        for (unsigned int i=0; i<count; i++) {
          if (table[2*i] == field) {
            _field = table[1+2*i];
            return;
          }
        }
        EEPROMWarnings += error + "\n";
      }
      else if (shift) {
        _field = field + shift;
      }
      else {
        _field = exportFunc(field);
      }
    }

    virtual void afterImport()
    {
      if (table) {
        field = 0;
        for (unsigned int i=0; i<count; i++) {
          if (table[1+2*i] == _field)
            field = table[2*i];
        }
      }
      else if (shift) {
        field = _field - shift;
      }
      else {
        field = importFunc(_field);
      }
    }

  protected:
    T internalField;
    int & field;
    int _field;
    const unsigned int count;
    const int * table;
    int shift;
    int (*exportFunc)(int);
    int (*importFunc)(int);
    const QString error;
};

#endif
