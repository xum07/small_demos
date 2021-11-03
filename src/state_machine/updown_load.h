#ifndef SFB_UPDOWNLOAD_H
#define SFB_UPDOWNLOAD_H

#include "sftm_mng.h"
#include "fb_factory.h"
#include "fb_fsm.h"
#include "dbms_if.h"
#include "interface_paras.h"
#include "dbsched/updown_load.h"

namespace DBMS {

class IDbmsUpDownload : public DBDist::StateMachine<IDbmsUpDownload> {
public:
    friend class DBDist::StateMachine<IDbmsUpDownload>;

    virtual uint32_t TryCheck(const TransEntry &) = 0;
    virtual uint32_t ReadyData(const TransReady &) = 0;
    virtual uint32_t TransData(const TransStart &) = 0;
    virtual uint32_t Rollback(const TransFail &) = 0;
    virtual uint32_t TryRollback(const TransFail &) = 0;
    virtual uint32_t TransEnd(const TransOver &) = 0;

    // define states and initialization state
    enum FSMStates {
        tryCheck, readyData, transData, rollback, transEnd, M_INITIAL_STATE = tryCheck
    };

    // define fsm table
    using TransitionTable = FSMTable(
        //         current,  event,      next,      action        
        Transition<tryCheck, TransEntry, readyData, &IDbmsUpDownload::TryCheck>,
        Transition<tryCheck, TransFail, transEnd, &IDbmsUpDownload::TryRollback>,
        Transition<readyData, TransReady, transData, &IDbmsUpDownload::ReadyData>,
        Transition<readyData, TransFail, rollback, &IDbmsUpDownload::Rollback>,
        Transition<transData, TransStart, transEnd, &IDbmsUpDownload::TransData>,
        Transition<transData, TransFail, rollback, &IDbmsUpDownload::Rollback>,
        Transition<rollback, TransOver, transEnd, &IDbmsUpDownload::TransEnd>
    );
};

class DbmsUpload : public IDbmsUpDownload, public IDbSchedSrvUpdownLoad {
public:
    DbmsUpload();
    virtual ~DbmsUpload() = default;

    DbmsUpload(const DbmsUpload &) = delete;
    DbmsUpload &operator=(const DbmsUpload &) = delete;
    DbmsUpload(DbmsUpload &&) = delete;
    DbmsUpload &operator=(DbmsUpload &&) = delete;

    uint32_t TryCheck(const TransEntry &) override;
    uint32_t ReadyData(const TransReady &) override;
    uint32_t TransData(const TransStart &) override;
    uint32_t Rollback(const TransFail &) override;
    uint32_t TryRollback(const TransFail &) override;
    uint32_t TransEnd(const TransOver &) override;
};

class DbmsDownload : public IDbmsUpDownload, public IDbSchedSrvUpdownLoad {
public:
    DbmsDownload();
    virtual ~DbmsDownload() = default;

    DbmsDownload(const DbmsDownload &) = delete;
    DbmsDownload &operator=(const DbmsDownload &) = delete;
    DbmsDownload(DbmsDownload &&) = delete;
    DbmsDownload &operator=(DbmsDownload &&) = delete;

    uint32_t TryCheck(const TransEntry &) override;
    uint32_t ReadyData(const TransReady &) override;
    uint32_t TransData(const TransStart &) override;
    uint32_t Rollback(const TransFail &) override;
    uint32_t TryRollback(const TransFail &) override;
    uint32_t TransEnd(const TransOver &) override;
};

} 
#endif