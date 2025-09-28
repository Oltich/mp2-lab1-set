#include "tbitfield.h"

TBitField::TBitField(int len) : BitLen(len)
{
  if (len <= 0)
  throw invalid_argument("Bit field length must be positive");

  MemLen = (len + sizeof(TELEM) * 8 - 1) / (sizeof(TELEM) * 8);
  pMem = new TELEM[MemLen];

  for (int i = 0; i < MemLen; i++)
  pMem[i] = 0;
}

TBitField::TBitField(const TBitField& bf) : BitLen(bf.BitLen), MemLen(bf.MemLen)
{
  pMem = new TELEM[MemLen];
  for (int i = 0; i < MemLen; i++)
  pMem[i] = bf.pMem[i];
}

TBitField::~TBitField()
{
  delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const
{
  if (n < 0 || n >= BitLen)
  throw out_of_range("Bit index out of range");

  return n / (sizeof(TELEM) * 8);
}

TELEM TBitField::GetMemMask(const int n) const
{
  if (n < 0 || n >= BitLen)
  throw out_of_range("Bit index out of range");

  return 1 << (n % (sizeof(TELEM) * 8));
}

int TBitField::GetLength(void) const
{
  return BitLen;
}

void TBitField::SetBit(const int n)
{
  int index = GetMemIndex(n);
  TELEM mask = GetMemMask(n);
  pMem[index] |= mask;
}

void TBitField::ClrBit(const int n)
{
  int index = GetMemIndex(n);
  TELEM mask = GetMemMask(n);
  pMem[index] &= ~mask;
}

int TBitField::GetBit(const int n) const
{
  int index = GetMemIndex(n);
  TELEM mask = GetMemMask(n);
  return (pMem[index] & mask) != 0;
}

TBitField& TBitField::operator=(const TBitField& bf)
{
  if (this != &bf)
  {
  if (BitLen != bf.BitLen)
  {
    delete[] pMem;
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem = new TELEM[MemLen];
  }

  for (int i = 0; i < MemLen; i++)
    pMem[i] = bf.pMem[i];
  }
  return *this;
}

int TBitField::operator==(const TBitField& bf) const
{
  if (BitLen != bf.BitLen)
  return 0;

  for (int i = 0; i < MemLen; i++)
  if (pMem[i] != bf.pMem[i])
    return 0;

  return 1;
}

int TBitField::operator!=(const TBitField& bf) const
{
  return !(*this == bf);
}

TBitField TBitField::operator|(const TBitField& bf)
{
  int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
  TBitField result(maxLen);

  for (int i = 0; i < MemLen; i++)
  result.pMem[i] = pMem[i];

  for (int i = 0; i < bf.MemLen; i++)
  result.pMem[i] |= bf.pMem[i];

  return result;
}
TBitField TBitField::operator&(const TBitField& bf)
{
  int maxLen = (BitLen > bf.BitLen) ? BitLen : bf.BitLen;
  TBitField result(maxLen);

  int minMemLen = (MemLen < bf.MemLen) ? MemLen : bf.MemLen;
  for (int i = 0; i < minMemLen; i++)
  result.pMem[i] = pMem[i] & bf.pMem[i];

  return result;
}

TBitField TBitField::operator~(void)
{
  TBitField result(BitLen);

  for (int i = 0; i < MemLen; i++)
  result.pMem[i] = ~pMem[i];

  int extraBits = BitLen % (sizeof(TELEM) * 8);
  if (extraBits != 0)
  {
  TELEM mask = (1 << extraBits) - 1;
  result.pMem[MemLen - 1] &= mask;
  }

  return result;
}

istream& operator>>(istream& istr, TBitField& bf)
{
  for (int i = 0; i < bf.BitLen; i++)
  {
  char bit;
  istr >> bit;
  if (bit == '1')
    bf.SetBit(i);
  else if (bit == '0')
    bf.ClrBit(i);
  else
  {
    istr.setstate(ios::failbit);
    break;
  }
  }
  return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf)
{
  for (int i = 0; i < bf.BitLen; i++)
  {
  ostr << (bf.GetBit(i) ? '1' : '0');
  }
  return ostr;
}