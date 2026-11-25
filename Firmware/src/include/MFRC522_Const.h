
#ifndef MFRC522_CONST_H
#define MFRC522_CONST_H

#include <stdint.h>

// lam facut namespace pentru ca sunt foarte multe;
namespace MFRC522_Const
{
    //////////////////////////////////////////////////////
    // MFRC522 - PAGE 0: COMMAND AND STATUS
    //////////////////////////////////////////////////////

    constexpr uint8_t CommandReg    = 0x01;
    constexpr uint8_t ComIrqReg     = 0x04;
    constexpr uint8_t DivIrqReg     = 0x05;
    constexpr uint8_t ErrorReg      = 0x06;
    constexpr uint8_t Status2Reg    = 0x08;
    constexpr uint8_t FIFODataReg   = 0x09;
    constexpr uint8_t FIFOLevelReg  = 0x0A;
    constexpr uint8_t ControlReg    = 0x0C;
    constexpr uint8_t BitFramingReg = 0x0D;
    constexpr uint8_t CollReg       = 0x0E;

    //////////////////////////////////////////////////////
    // MFRC522 - PAGE 1: COMMAND
    //////////////////////////////////////////////////////

    constexpr uint8_t ModeReg      = 0x11;
    constexpr uint8_t TxControlReg = 0x14;
    constexpr uint8_t TxASKReg     = 0x15;

    //////////////////////////////////////////////////////
    // MFRC522 - PAGE 2: CONFIGURATION
    //////////////////////////////////////////////////////

    constexpr uint8_t CRCResultRegH = 0x21;
    constexpr uint8_t CRCResultRegL = 0x22;
    constexpr uint8_t TReloadRegL   = 0x2D;

    //////////////////////////////////////////////////////
    // MFRC522 - PAGE 3: TEST
    //////////////////////////////////////////////////////

    constexpr uint8_t VersionReg = 0x37;

    //////////////////////////////////////////////////////
    // COMENZI INTERNE MFRC522 - PCD
    //////////////////////////////////////////////////////

    constexpr uint8_t PCD_Idle       = 0x00;
    constexpr uint8_t PCD_CalcCRC    = 0x03;
    constexpr uint8_t PCD_Transceive = 0x0C;
    constexpr uint8_t PCD_MFAuthent  = 0x0E;

    //////////////////////////////////////////////////////
    // COMENZI CARD - PICC
    //////////////////////////////////////////////////////

    constexpr uint8_t PICC_REQA          = 0x26;
    constexpr uint8_t PICC_SEL_CL1       = 0x93;
    constexpr uint8_t PICC_MF_AUTH_KEY_A = 0x60;
    constexpr uint8_t PICC_MF_AUTH_KEY_B = 0x61;
    constexpr uint8_t PICC_MF_READ       = 0x30;
    constexpr uint8_t PICC_MF_WRITE      = 0xA0;

    //////////////////////////////////////////////////////
    // NVB - NUMBER OF VALID BITS
    //////////////////////////////////////////////////////

    constexpr uint8_t NVB_Anticollision = 0x20;
    constexpr uint8_t NVB_Select = 0x70;

    //////////////////////////////////////////////////////
    // SPI - FORMATUL ADRESEI
    //////////////////////////////////////////////////////

    constexpr uint8_t SPI_ReadMask     = 0x80;
    constexpr uint8_t SPI_AddressMask  = 0x3F;
    constexpr uint8_t SPI_AddressShift = 1;
    constexpr uint8_t SPI_DummyByte    = 0x00;

    //////////////////////////////////////////////////////
    // ComIrqReg - INTERRUPT REQUEST BITS
    //////////////////////////////////////////////////////

    constexpr uint8_t TimerIRq   = (1U << 0);
    constexpr uint8_t ErrIRq     = (1U << 1);
    constexpr uint8_t LoAlertIRq = (1U << 2);
    constexpr uint8_t HiAlertIRq = (1U << 3);
    constexpr uint8_t IdleIRq    = (1U << 4);
    constexpr uint8_t RxIRq      = (1U << 5);
    constexpr uint8_t TxIRq      = (1U << 6);

    // Masti compuse pentru verificarea intreruperilor;
    constexpr uint8_t ComIrq_ErrorOrTimer = ErrIRq | TimerIRq;
    constexpr uint8_t ComIrq_TransceiveEvents = RxIRq | ErrIRq | TimerIRq;
    constexpr uint8_t ComIrq_AuthenticationEvents = IdleIRq | ErrIRq | TimerIRq;

    // Set1 = 0, bitii 6:0 = 1;
    // Stergem toate intreruperile memorate;
    constexpr uint8_t ComIrq_ClearAll = 0x7F;

    //////////////////////////////////////////////////////
    // DivIrqReg
    //////////////////////////////////////////////////////

    constexpr uint8_t CRCIRq = (1U << 2);

    //////////////////////////////////////////////////////
    // ErrorReg
    //////////////////////////////////////////////////////

    constexpr uint8_t ProtocolErr = (1U << 0);
    constexpr uint8_t ParityErr   = (1U << 1);
    constexpr uint8_t CRCErr      = (1U << 2);
    constexpr uint8_t CollErr     = (1U << 3);
    constexpr uint8_t BufferOvfl  = (1U << 4);

    // Bitul 5 este rezervat;
    constexpr uint8_t TempErr = (1U << 6);
    constexpr uint8_t WrErr = (1U << 7);

    // Toate erorile definite in ErrorReg: 0xDF;
    constexpr uint8_t Error_All = ProtocolErr | ParityErr | CRCErr | CollErr | BufferOvfl | TempErr | WrErr;

    // Toate erorile, exceptand CRCErr: 0xDB;
    constexpr uint8_t Error_WithoutCRC = ProtocolErr | ParityErr | CollErr | BufferOvfl | TempErr | WrErr;

    //////////////////////////////////////////////////////
    // Status2Reg - CRYPTO1
    //////////////////////////////////////////////////////

    constexpr uint8_t MFCrypto1On = (1U << 3);

    constexpr uint8_t ClearCrypto1 = static_cast<uint8_t>(~MFCrypto1On);

    //////////////////////////////////////////////////////
    // FIFOLevelReg
    //////////////////////////////////////////////////////

    constexpr uint8_t FlushBuffer = (1U << 7);

    //////////////////////////////////////////////////////
    // ControlReg
    //////////////////////////////////////////////////////

    constexpr uint8_t RxLastBitsMask = 0x07;

    //////////////////////////////////////////////////////
    // BitFramingReg
    //////////////////////////////////////////////////////

    constexpr uint8_t TxFullByte = 0x00;
    constexpr uint8_t Tx7Bits    = 0x07;
    constexpr uint8_t StartSend  = (1U << 7);

    constexpr uint8_t StartSend7Bits = StartSend | Tx7Bits;

    //////////////////////////////////////////////////////
    // CollReg
    //////////////////////////////////////////////////////

    constexpr uint8_t ValuesAfterColl = (1U << 7);

    //////////////////////////////////////////////////////
    // ModeReg - CRC PRESET
    //////////////////////////////////////////////////////

    constexpr uint8_t CRCPresetMask = 0x03;

    constexpr uint8_t ClearCRCPreset = static_cast<uint8_t>(~CRCPresetMask);

    constexpr uint8_t CRCPreset_0000 = 0x00;
    constexpr uint8_t CRCPreset_6363 = 0x01;
    constexpr uint8_t CRCPreset_A671 = 0x02;
    constexpr uint8_t CRCPreset_FFFF = 0x03;

    //////////////////////////////////////////////////////
    // TxControlReg - RF ANTENNA DRIVERS
    //////////////////////////////////////////////////////

    constexpr uint8_t Tx1RFEn = (1U << 0);
    constexpr uint8_t Tx2RFEn = (1U << 1);

    constexpr uint8_t RFEnable = Tx1RFEn | Tx2RFEn;

    constexpr uint8_t RFDisableMask = static_cast<uint8_t>(~RFEnable);

    //////////////////////////////////////////////////////
    // TxASKReg
    //////////////////////////////////////////////////////

    constexpr uint8_t Force100ASK = (1U << 6);

    //////////////////////////////////////////////////////
    // MIFARE CLASSIC - ACK
    //////////////////////////////////////////////////////

    constexpr uint8_t MIFARE_ACK           = 0x0A;
    constexpr uint8_t MIFARE_ACK_Mask      = 0x0F;
    constexpr uint8_t MIFARE_ACK_ValidBits = 4;

    //////////////////////////////////////////////////////
    // MIFARE CLASSIC - TRANSPORT ACCESS BITS
    //////////////////////////////////////////////////////

    constexpr uint8_t TransportAccessByte6 = 0xFF;
    constexpr uint8_t TransportAccessByte7 = 0x07;
    constexpr uint8_t TransportAccessByte8 = 0x80;

    //////////////////////////////////////////////////////
    // VERSION REG - VERSIUNI OFICIALE NXP
    //////////////////////////////////////////////////////

    constexpr uint8_t Version_NXP_1_0 = 0x91;
    constexpr uint8_t Version_NXP_2_0 = 0x92;

    //////////////////////////////////////////////////////
    // VERSION REG - ALTE VALORI ACCEPTATE
    //////////////////////////////////////////////////////

    constexpr uint8_t Version_Other_82 = 0x82;
    constexpr uint8_t Version_Other_90 = 0x90;
    constexpr uint8_t Version_Other_88 = 0x88;
    constexpr uint8_t Version_Other_89 = 0x89;
    constexpr uint8_t Version_Other_B2 = 0xB2;

    //////////////////////////////////////////////////////
    // CONSTANTE PENTRU TESTAREA SPI
    //////////////////////////////////////////////////////

    constexpr uint8_t SPI_TestPattern1   = 0x55;
    constexpr uint8_t SPI_TestPattern2   = 0xAA;
    constexpr uint8_t SPI_ReadErrorValue = 0xFF;

    //////////////////////////////////////////////////////
    // CONSTANTE SOFTWARE SI TEMPORIZARI                
    //////////////////////////////////////////////////////

    constexpr uint32_t SPI_TimeoutMs   = 100U;
    constexpr uint32_t RFID_TimeoutMs  = 30U;
    constexpr uint32_t RF_ResetDelayMs = 50U;

    constexpr uint8_t FIFO_Capacity = 64;
}

#endif
