#include "dbsched_uts_adapt.h"
#include <utility>
#include "dbms_log.h"

using namespace std;
using namespace DBMS;

DWORD DbmsTran::PreProcess(DWORD tranType)
{
    return FsmExecute(tranType, DBTRANS_PRE_PROCESS);
}

DWORD DbmsTran::MidProcess()
{
    return FsmExecute(static_cast<uint32_t>(-1), DBTRANS_MID_PROCESS);
}
DWORD DbmsTran::PostProcess()
{
    return FsmExecute(static_cast<uint32_t>(-1), DBTRANS_POST_PROCESS);
}

DWORD DbmsTran::GetFileAll(CSftmFileNode **,  DWORD &)
{
    return DBMS_SUCCESS;
}

uint32_t DbmsTran::CheckFile(const CSftmFileNode *, uint32_t)
{
    return DBMS_SUCCESS;
}

DWORD DbmsTran::ContructSynDbBuff(T_SynDB_Head &, BYTE **, DWORD &)
{
    return DBMS_SUCCESS;
}

DWORD DbmsTran::ParaseSynDbBuff(DWORD, BYTE *, DWORD)
{
    return DBMS_SUCCESS;
}

DWORD DbmsTran::GetPrepareState(DWORD &)
{
    return DBMS_SUCCESS;
}

BOOL DbmsTran::IsVrpDBDownload(const CHAR *)
{
    return TRUE;
}

DWORD DbmsTran::DbulUpdatePkgFile(CHAR *, COspVector &)
{
    return DBMS_SUCCESS;
}

DWORD DbmsTran::DbdlProcPkgFile(CHAR *)
{
    return DBMS_SUCCESS;
}

DWORD DbmsTran::DbdlCheckFile(CHAR *)
{
    return DBMS_SUCCESS;
}

uint32_t DbmsTran::FsmExecute(uint32_t tranType, uint32_t stage) const
{
    ASSERT(stage <= FAIL_PROCESS);

    auto fsm = GetFsm(tranType);
    if (fsm == nullptr) {
        return DBMS_FAIL;
    }

    std::unique_ptr<IDbmsUpDownload> uploadFb(fsm);
    static std::map<uint32_t, std::function<uint32_t(IDbmsUpDownload &)>> stage2action = {
        {DBTRANS_PRE_PROCESS, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransEntry()); }}, // fsm::tryCheck
        {DBTRANS_PRE_CFAIL, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransFail()); }},
        {DBTRANS_PRE_READY, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransReady()); }}, // fsm::readyData
        {DBTRANS_PRE_RFAIL, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransFail()); }},
        {DBTRANS_MID_PROCESS, [](IDbmsUpDownload &obj) { return DBMS_SUCCESS; }},
        {DBTRANS_POST_PROCESS, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransStart()); }}, // fsm::transData
        {DBTRANS_POST_FAIL, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransFail()); }},
        {DBTRANS_POST_OVER, [](IDbmsUpDownload &obj) { return obj.ProcessEvent(TransOver()); }} // fsm::rollback
    };

    uint32_t fsmState = stage;
    while (fsmState != DBMS_SUCCESS && fsmState != DBTRANS_INVALID) {
        fsmState = stage2action[fsmState](*uploadFb);
    }
    return fsmState;
}

IDbmsUpDownload* DbmsTran::GetFsm(uint32_t tranType) const
{
    if (tranType == SFTM_DBFUL_MAIN_TYPE) {
        return new DbmsUpload;
    } else if (tranType == SFTM_DBFDL_MAIN_TYPE) {
        return new DbmsDownload;
    } else {
        CDbmsLog::DbmsCycleLog("DbmsTran::FsmExecute type error: %u", tranType);
        return nullptr;
    }
}
