/** @file queuedPVSetImpl.h
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

extern int queuedPVSetImpl(const char* recname, const char* pv_in, epicsUInt32 len_pv_in, epicsUInt32 max_len_pv_in, const char* value_in, epicsUInt32 max_len_value_in, epicsUInt32 len_value_in, double timeout);

#ifdef __cplusplus
};
#endif
