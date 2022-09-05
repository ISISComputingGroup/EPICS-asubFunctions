#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>

#include "epicsThread.h"
#include "splitCharWaveform.c"
#include "gtest/gtest.h"

namespace {

    class SplitCharWaveformTest : public ::testing::Test {
    protected:
        const int bVal = 9;
        const int cVal = 2;
        const int eVal = 5;
        const epicsEnum16 ftvVal = 0;

        aSubRecord prec;
        ptrdiff_t step;
        int valuVal = 19;
        epicsUInt32 nov = 1;
        epicsOldString valaValue[1] = { "word_word" };
        epicsOldString valbValue[1] = { "" };
        epicsOldString valVals[1] = { "" };

        void setupASubRecord()
        {
            strcpy(prec.name, "name");
            prec.fta = menuFtypeCHAR;
            prec.ftb = menuFtypeULONG;
            prec.ftc = menuFtypeULONG;
            prec.ftd = menuFtypeSTRING;
            prec.fte = menuFtypeULONG;
            prec.ftvu = menuFtypeULONG;
            prec.a = "word_word";
            prec.b = (void*)&bVal;
            prec.c = (void*)&cVal;
            prec.d = "_";
            prec.e = (void*)&eVal;
            prec.noa = 14;
            prec.vala = valaValue;
            prec.valb = valbValue;
            prec.valu = &valuVal;

            step = &(prec.ftvb) - &(prec.ftva);

            for (int i = 0; i < 20; ++i)
            {
                epicsEnum16* rec_val_ptr = &(prec.ftva) + i * step;
                *rec_val_ptr = ftvVal;
            }

            step = &(prec.novb) - &(prec.nova);

            for (int i = 0; i < 20; ++i)
            {
                epicsUInt32* rec_val_ptr = &(prec.nova) + i * step;
                *rec_val_ptr = nov;
            }

            step = &(prec.valb) - &(prec.vala);

            for (int i = 2; i < 20; ++i)
            {
                void** rec_val_ptr = &(prec.vala) + i * step;
                *rec_val_ptr = valVals;
            }
        }

        void SetUp() override {
            memset(&prec, 0, sizeof(prec));
            setupASubRecord();
        }
    };

    TEST_F(SplitCharWaveformTest, test_GIVEN_waveform_in_mode_2_WHEN_split_wave_form_THEN_split_on_str_len) {
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
        ASSERT_STRCASEEQ(valcResult[0], "");
    }

    
    TEST_F(SplitCharWaveformTest, test_GIVEN_waveform_in_mode_1_WHEN_split_wave_form_THEN_split_on_delim) {
        // Given
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
        ASSERT_STRCASEEQ(valcResult[0], "");
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_waveform_in_mode_1_wrong_fte_type_WHEN_split_wave_form_THEN_split_on_delim) {
        // Given
        prec.fte = menuFtypeDOUBLE;
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
        ASSERT_STRCASEEQ(valcResult[0], "");
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_fta_type_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.fta = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_ftb_type_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.ftb = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_ftc_type_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.ftc = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_ftd_type_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.ftd = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_fte_type_in_mode_2_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.fte = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_ftvu_type_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.ftvu = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_ftv_type_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.ftvb = menuFtypeDOUBLE;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }

    TEST_F(SplitCharWaveformTest, test_GIVEN_incorrect_nov_size_WHEN_split_wave_form_THEN_error_status_is_returned) {
        // Given
        prec.novb = 0;

        // When:
        long status = splitCharWaveform(&prec);

        // Then:
        ASSERT_EQ(status, -1);
    }
}
