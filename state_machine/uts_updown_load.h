#ifndef DBSCHED_UTS_ADAPT_H
#define DBSCHED_UTS_ADAPT_H

#include <memory>
#include "sftm_mng.h"
#include "sfb_updownload.h"

enum DBTransStage : uint32_t {
    DBTRANS_PRE_PROCESS = 0,
    DBTRANS_PRE_READY,
    DBTRANS_PRE_CFAIL,
    DBTRANS_PRE_RFAIL,
    DBTRANS_MID_PROCESS,
    DBTRANS_POST_PROCESS,
    DBTRANS_POST_FAIL,
    DBTRANS_POST_OVER,
    DBTRANS_INVALID
};

// this class should be implemented in the dbsched_uts_adapt
class DbmsTran : public CSftmTranBase {
public:
    DbmsTran() = default;
    virtual ~DbmsTran() = default;

    DbmsTran(const DbmsTran &) = delete;
    DbmsTran &operator=(const DbmsTran &) = delete;
    DbmsTran(DbmsTran &&) = delete;
    DbmsTran &operator=(DbmsTran &&) = delete;

    DWORD PreProcess(DWORD) override;
    DWORD MidProcess() override;
    DWORD PostProcess() override;

    DWORD GetFileAll(CSftmFileNode **,  DWORD &) override;
    uint32_t CheckFile(const CSftmFileNode *, uint32_t) override;
    DWORD ContructSynDbBuff(T_SynDB_Head &, BYTE **, DWORD &) override;
    DWORD ParaseSynDbBuff(DWORD, BYTE *, DWORD) override;
    DWORD GetPrepareState(DWORD &) override;
    BOOL IsVrpDBDownload(const CHAR *) override; 
    DWORD DbulUpdatePkgFile(CHAR *, COspVector &) override;
    DWORD DbdlProcPkgFile(CHAR *) override;
    DWORD DbdlCheckFile(CHAR *) override;

private:
    uint32_t FsmExecute(uint32_t tranType, uint32_t stage) const;
    DBMS::IDbmsUpDownload* GetFsm(uint32_t tranType) const;
};

#endif