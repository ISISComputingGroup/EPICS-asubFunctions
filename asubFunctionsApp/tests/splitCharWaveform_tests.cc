#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>

#include "epicsThread.h"
#include "splitCharWaveForm.c"
#include "gtest/gtest.h"

namespace {
    epicsUInt32 nov = 1;
    int bVal = 9;
    int cVal = 2;
    int valuVal = 19;
    int eVal = 5;
    epicsEnum16 ftvVal = 0;

    epicsOldString valaValue[1] = { "word_word" };
    epicsOldString valbValue[1] = { "" };
    epicsOldString valVals[1] = { "" };
    epicsOldString resultA = "word";
    epicsOldString resultB = "_word";
    epicsOldString resultC = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    static void setupASubRecord(aSubRecord* prec)
    {
        strcpy(prec->name, "name");
        prec->fta = menuFtypeCHAR;
        prec->ftb = menuFtypeULONG;
        prec->ftc = menuFtypeULONG;
        prec->ftd = menuFtypeSTRING;
        prec->fte = menuFtypeULONG;
        prec->ftvu = menuFtypeULONG;
        prec->a = "word_word";
        prec->b = &bVal;
        prec->c = &cVal;
        prec->d = "_";
        prec->e = &eVal;
        prec->noa = 14;
        prec->vala = valaValue;
        prec->valb = valbValue;
        prec->valc = valVals;
        prec->vald = valVals;
        prec->vale = valVals;
        prec->valf = valVals;
        prec->valg = valVals;
        prec->valh = valVals;
        prec->vali = valVals;
        prec->valj = valVals;
        prec->valk = valVals;
        prec->vall = valVals;
        prec->valm = valVals;
        prec->valn = valVals;
        prec->valo = valVals;
        prec->valp = valVals;
        prec->valq = valVals;
        prec->valr = valVals;
        prec->vals = valVals;
        prec->valt = valVals;
        prec->valu = &valuVal;

        for (int i = 0; i < 20; ++i)
        {
            ptrdiff_t step = &(prec->ftvb) - &(prec->ftva);
            epicsEnum16* rec_val_ptr = &(prec->ftva) + i * step;
            *rec_val_ptr = ftvVal;
        }

        for (int i = 0; i < 20; ++i)
        {
            ptrdiff_t step = &(prec->novb) - &(prec->nova);
            epicsUInt32* rec_val_ptr = &(prec->nova) + i * step;
            *rec_val_ptr = nov;
        }
    }

    TEST(SplitCharWaveformTest, splits_wave_form_mode_2) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);

        // When:
        long status = splitCharWaveform(&prec);

        epicsOldString* valaResult = (epicsOldString*)(prec.vala);
        epicsOldString* valbResult = (epicsOldString*)(prec.valb);
        epicsOldString* valcResult = (epicsOldString*)(prec.valc);
        epicsUInt32 valuResult = *(epicsUInt32*)(prec.valu);

        // Then:
        ASSERT_EQ(status, 0);
        ASSERT_EQ(valuResult, 2);
        ASSERT_STRCASEEQ(valaResult[0], "word_");
        ASSERT_STRCASEEQ(valbResult[0], "word");
        ASSERT_STRCASEEQ(valcResult[0], "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
    }

    
    TEST(SplitCharWaveformTest, splits_wave_form_mode_1) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.c = &nov;

        // When:
        long status = splitCharWaveform(&prec);

        epicsOldString* valaResult = (epicsOldString*)(prec.vala);
        epicsOldString* valbResult = (epicsOldString*)(prec.valb);
        epicsOldString* valcResult = (epicsOldString*)(prec.valc);
        epicsUInt32 valuResult = *(epicsUInt32*)(prec.valu);

        // Then:
        ASSERT_EQ(status, 0);
        ASSERT_EQ(valuResult, 2);
        ASSERT_STRCASEEQ(valaResult[0], "word");
        ASSERT_STRCASEEQ(valbResult[0], "word");
        ASSERT_STRCASEEQ(valcResult[0], "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_fta_type) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.fta = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_ftb_type) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftb = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_ftc_type) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftc = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_ftd_type) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftd = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }
    TEST(SplitCharWaveformTest, rejects_incorrect_fte_type_in_mode_not_1) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.fte = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST(SplitCharWaveformTest, ignores_incorrect_fte_type_in_mode_1) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.fte = menuFtypeDOUBLE;
        prec.c = &nov;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, 0);
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_ftvu_type) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftvu = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_ftv_type) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.ftvb = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST(SplitCharWaveformTest, rejects_incorrect_nov_size) {
        // Given
        aSubRecord prec;
        memset(&prec, 0, sizeof(prec));
        setupASubRecord(&prec);
        prec.novb = 0;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }
}