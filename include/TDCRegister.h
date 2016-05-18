#ifndef TDCRegister_h
#define TDCRegister_h

#include <vector>
#include <math.h> // ceil
#include <stdint.h>

#define WORD_SIZE 8

/**
 * \brief General register object to interact with a HPTDC chip
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 24 Apr 2015
 *
 * \ingroup HPTDC
 */
class TDCRegister
{
  public:
    /// LSB index
    typedef uint16_t bit;
    /// Unit of the TDC register word to be successfully contained on any machine
    typedef uint32_t word_t;

  public:
    inline TDCRegister(const unsigned int size) :
      fNumWords(ceil(size/WORD_SIZE)+1), fWordSize(size) {
      fWord = new word_t[fNumWords];
      Clear();
    }
    inline TDCRegister(const unsigned int size, const TDCRegister& r) :
      fNumWords(ceil(size/WORD_SIZE)+1), fWordSize(size) {
      fWord = new word_t[fNumWords];
      for (unsigned int i=0; i<GetNumWords(); i++) { fWord[i] = r.fWord[i]; }
    }
    inline TDCRegister(const unsigned int size, const std::vector<uint8_t> words) :
      fNumWords(ceil(size/WORD_SIZE)+1), fWordSize(size) {
      fWord = new word_t[fNumWords];
      for (unsigned int i=0; i<words.size(); i++) { SetWord(i, words[i]); }
    }
    inline virtual ~TDCRegister() {
      if (fWord) delete [] fWord;
    }
    inline TDCRegister& operator=(const TDCRegister& r) {
      if (&r!=this) {
        fWordSize = r.fWordSize;
        fNumWords = r.fNumWords;
        word_t* newWord = fNumWords ? new word_t[fNumWords] : 0;
        std::copy(r.fWord, r.fWord+fNumWords, newWord);
        // replace the old data (all are non-throwing)
        delete [] fWord;
        fWord = newWord;
      }
      return *this;
    }
    
    /// Set one bit(s) subset in the register word
    inline void SetWord(const unsigned int i, const word_t word) {
      if (i<0 or i>=fNumWords) return;
      fWord[i] = word;
    }
    /// Retrieve one subset from the register word
    inline word_t GetWord(const unsigned int i) const {
      if (i<0 or i>=fNumWords) return -1;
      return fWord[i];
    }
    inline word_t* GetWords() const { return fWord; }
    /**
     * Return the number of words making up the full register word.
     * \brief Number of words in the register
     */
    inline uint8_t GetNumWords() const { return fNumWords; }
    
    inline void DumpRegister(std::ostream& os=std::cout, const bit max_bits=-1) const {
      os << std::endl;
      for (unsigned int i=0; i<fNumWords; i++) {
        os << " Word " << std::setw(2) << i << ":  "
           << std::setw(3) << std::dec << (i+1)*WORD_SIZE-1 << "-> |";
        for(int8_t j=WORD_SIZE-1; j>=0; j--) {
          uint16_t bit = j+i*WORD_SIZE;
          // bits values
          if (bit>fWordSize or (max_bits>0 and bit>=max_bits)) os << "x";
          else os << static_cast<bool>((fWord[i] & static_cast<word_t>(1<<j))>>j);
          // delimiters
          if (j%16==0 && j!=0) os << "| |";
          else if (j%8==0) os << "|";
          else if (j%4==0) os << " ";
        }
        os << " <-" << std::setw(3) << i*WORD_SIZE 
           << " -=- 0x" << std::hex << std::setw(4) << std::setfill('0') << fWord[i] << std::dec << std::endl;
      }
      os << std::endl;
    }
    /// Ensure that the critical constant values are properly set in the
    /// register word
    virtual void SetConstantValues()=0;
    
  protected:
    /**
     * Set a fixed amount of bits in the full register word
     * \brief Set bits in the register word
     * \param[in] lsb Least significant bit of the word to set
     * \param[in] word Word to set
     * \param[in] size Size of the word to set
     */
    inline void SetBits(uint16_t lsb, uint16_t word, uint8_t size) {
      if (size<=0 or size>16) return;
      //FIXME FIXME FIXME burp...
      // See http://www.ioccc.org/ for more information
      for (uint8_t i=0; i<size; i++) {
        uint16_t bit = lsb+i;
        uint8_t bit_rel = bit % WORD_SIZE;
        uint8_t word_id = (bit-bit_rel)/WORD_SIZE;
        fWord[word_id] &=~(1<<bit_rel); // first we clear the bit
        fWord[word_id] |= (((word>>i)&0x1)<<bit_rel); // then we set it
      }
    }
    /**
     * Extract a fixed amount of bits from the full register word
     * \brief Extract bits from the register word
     * \param[in] lsb Least significant bit of the word to retrieve
     * \param[in] size Size of the word to retrieve
     */
    inline uint16_t GetBits(uint16_t lsb, uint8_t size) const {
      if (size<=0 or size>16) return -1;
      uint16_t out = 0x0;
      for (uint8_t i=0; i<size; i++) {
        uint16_t bit = lsb+i;
        uint8_t bit_rel = bit % WORD_SIZE;
        uint8_t word_id = (bit-bit_rel)/WORD_SIZE;
        out |= (((fWord[word_id]>>bit_rel)&0x1)<<i);
      }
      return out;
    }
    /// Set all bits in this register to '0'
    inline void Clear() {
      for (uint8_t i=0; i<fNumWords; i++) { fWord[i] = 0; }
    }
    
    /// Pointer to this register's word
    word_t* fWord;
    /// Number of words to fit the \a fWordSize bits of this register to this
    /// object
    unsigned int fNumWords;
    /// Number of bits in this register
    unsigned int fWordSize;
};

#endif
