#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <epicsExport.h>
#include <errlog.h>
#include <menuFtype.h>
#include <gsl/gsl_fit.h>

/**
 * aSub wrapper around gsl_fit_linear.
 *
 * Expected inputs:
 *   A - Y data waveform (doubles)
 *   NOA - Number of elements in Y (must be equal to NOB)
 *   B - X data waveform (doubles)
 *   NOB - Number of elements in X (must be equal to NOA)
 *
 * Outputs (for detailed descriptions of these outputs, see https://www.gnu.org/software/gsl/doc/html/lls.html#c.gsl_fit_linear ):
 *   VALA - c0 fitting parameter (model is Y = c0 + (c1 * X) )
 *   VALB - c1 fitting parameter (model is Y = c0 + (c1 * X) )
 *   VALC - variance-covariance matrix parameter c00
 *   VALD - variance-covariance matrix parameter c01
 *   VALE - variance-covariance matrix parameter c11
 *   VALF - The weighted sum of squares of the residuals from the best-fit line (chi-squared)
 */
static long line_of_best_fit(aSubRecord *prec)
{
    int status;
    
    if (prec->fta != menuFtypeDOUBLE || prec->ftb != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "line_of_best_fit: input types incorrect");
        return 1;
    }
    
    if (prec->ftva != menuFtypeDOUBLE || prec->ftvb != menuFtypeDOUBLE || prec->ftvc != menuFtypeDOUBLE || prec->ftvd != menuFtypeDOUBLE || prec->ftve != menuFtypeDOUBLE || prec->ftvf != menuFtypeDOUBLE)
    {
        errlogSevPrintf(errlogMajor, "line_of_best_fit: output types incorrect");
        return 1;
    }
    
    if (prec->noa != prec->nob) {
        errlogSevPrintf(errlogMajor, "line_of_best_fit: cannot calculate best fit as noa != nob");
        return 1;
    }
    
    status = gsl_fit_linear(
        (double*) (prec->b),    // x data
        1,                      // x stride
        (double*) (prec->a),    // y data
        1,                      // y stride
        prec->noa,              // Number of elements
        (double*) (prec->vala), // c0 fitting parameter
        (double*) (prec->valb), // c1 fitting parameter
        (double*) (prec->valc), // c00 variance-covariance matrix parameter
        (double*) (prec->vald), // c01 variance-covariance matrix parameter
        (double*) (prec->vale), // c11 variance-covariance matrix parameter
        (double*) (prec->valf)  // chi-squared
    );
    
    if (status) {
        errlogSevPrintf(errlogMajor, "line_of_best_fit: GSL fit error: status = %d", status);
    }
    return status;
}

epicsRegisterFunction(line_of_best_fit);
