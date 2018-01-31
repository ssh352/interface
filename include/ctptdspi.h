#ifndef CTP_TRADE_API_SPI_H
#define CTP_TRADE_API_SPI_H


#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

#include "utility.h"
#include "ctpif_global.h"
#include "ctpif_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

typedef CThostFtdcInstrumentField INSTRUMENT;
typedef CThostFtdcTradingAccountField ACCOUNT;
typedef CThostFtdcOrderField CTPORDER;
typedef CThostFtdcTradeField CTPTRADE;
typedef CThostFtdcSettlementInfoConfirmField SETTLEMENT;
typedef CThostFtdcInputOrderActionField CTPRECALLORDER;

typedef struct TdifSessionInfo {
  /// 交易日期 ///
  TThostFtdcDateType TradingDay;
  /// 登录成功时间 ///
  TThostFtdcTimeType LoginTime;
  /// 经纪公司代码 ///
  TThostFtdcBrokerIDType BrokerID;
  /// 用户代码 ///
  TThostFtdcUserIDType UserID;
  /// 交易系统名称 ///
  TThostFtdcSystemNameType SystemName;
  /// 前置编号 ///
  TThostFtdcFrontIDType FrontID;
  /// 会话编号 ///
  TThostFtdcSessionIDType SessionID;
  /// 最大报单引用编号 ///
  int MaxOrderRef;
  /// 上期所时间 ///
  int m_nDeltaSHFETime;
  /// 大商所时间 ///
  int m_nDeltaDCETime;
  /// 郑商所时间 ///
  int m_nDeltaCZCETime;
  /// 中金所时间 ///
  int m_nDeltaFFEXTime;
} TdifSessionInfo;

typedef std::vector<struct CThostFtdcInstrumentField*> VecInstrument;

class CTPIF_API CtpTradeSpi : public CThostFtdcTraderSpi {
public:
  CtpTradeSpi(GmdParam* gmd_param, const char *logpath);
  virtual ~CtpTradeSpi();

  bool is_running() { return ctp_ != 0; }

  int run();
  int stop();
  int login();
  int logout();

  int query_instrument(const char *instrument = NULL);
  int query_account();
  int query_investor_position(const char *instrument = NULL);
  int query_order(const char *instrument = NULL, const char *exchange = NULL,
                  const char *ordersysid = NULL, const char *begintime = NULL,
                  const char *endtime = NULL);
  int query_trade(const char *instrument = NULL, const char *exchange = NULL,
                  const char *tradeid = NULL, const char *begintime = NULL,
                  const char *endtime = NULL);

  int order_insert_limit_price(const char *instrument, char cDirection,
                               double fPrice, int nVolume,
                               char cVolumeCondition, const char *strGTDdate,
                               char cTimeCondition, int nMinVolume,
                               char cTrigerCondition, double fStopPrice);

  int order_insert_limitd(const char *instrument, char cDirection,
                          double fPrice, int nVolume);
  int order_insert_condtn(const char *instrument, char cDirection,
                          double fPrice, int nVolume, double fConditionPrice,
                          char cTrigerCondition, char cPriceCondition);
  int order_corver_condtn(const char *instrument, char cDirection,
                          double fPrice, int nVolume, double fConditionPrice,
                          char cTrigerCondition, char cPriceCondition);
  int order_insert_parked();
  int order_corver(const char *instrument, char cDirection, double fPrice,
                   int nVolume);
  int order_recall(CTPORDER *order);

  char *GetOrderStatus(char status, char *buf, int len);

  bool is_user_logined() { return logined_; }

  int prepare_environment();


public:
  const VecInstrument& get_vector_instrument() { return vecinst_; }

protected:
  CThostFtdcTraderApi *ctp_ = nullptr;
  int nRequestID_ = 0;
  TdifSessionInfo SessionInfo_;
  char logpath_[MAX_PATH] = {""};
  bool logined_ = false;
public:
  GmdParam* gmd_param_ = nullptr;
protected:
  int confirm_settlement_info();
  void set_session(CThostFtdcRspUserLoginField *pRspUserLogin);

  // CThostFtdcTraderSpi interface
public:
  void OnFrontConnected();
  void OnFrontDisconnected(int nReason);
  void OnHeartBeatWarning(int nTimeLapse);
  void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast);
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast);
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast);
  void OnRspUserPasswordUpdate(
      CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspTradingAccountPasswordUpdate(
      CThostFtdcTradingAccountPasswordUpdateField *
          pTradingAccountPasswordUpdate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
  void
  OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast);
  void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspQueryMaxOrderVolume(
      CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void
  OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast);
  void OnRspRemoveParkedOrderAction(
      CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
  void OnRspExecOrderAction(
      CThostFtdcInputExecOrderActionField *pInputExecOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
  void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspBatchOrderAction(
      CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast);
  void OnRspQryOrder(CThostFtdcOrderField *pOrder,
                     CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                     bool bIsLast);
  void OnRspQryTrade(CThostFtdcTradeField *pTrade,
                     CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                     bool bIsLast);
  void
  OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
  void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
  void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
  void OnRspQryInstrumentMarginRate(
      CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryInstrumentCommissionRate(
      CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryExchange(CThostFtdcExchangeField *pExchange,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspQryProduct(CThostFtdcProductField *pProduct,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast);
  void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
  void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast);
  void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
  void OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
  void OnRspQryInvestorPositionDetail(
      CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryNotice(CThostFtdcNoticeField *pNotice,
                      CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                      bool bIsLast);
  void OnRspQrySettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryInvestorPositionCombineDetail(
      CThostFtdcInvestorPositionCombineDetailField *
          pInvestorPositionCombineDetail,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryCFMMCTradingAccountKey(
      CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
  void OnRspQryInvestorProductGroupMargin(
      CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryExchangeMarginRate(
      CThostFtdcExchangeMarginRateField *pExchangeMarginRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryExchangeMarginRateAdjust(
      CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
  void OnRspQrySecAgentACIDMap(CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast);
  void OnRspQryProductExchRate(CThostFtdcProductExchRateField *pProductExchRate,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast);
  void OnRspQryProductGroup(CThostFtdcProductGroupField *pProductGroup,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
  void OnRspQryMMInstrumentCommissionRate(
      CThostFtdcMMInstrumentCommissionRateField *pMMInstrumentCommissionRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryMMOptionInstrCommRate(
      CThostFtdcMMOptionInstrCommRateField *pMMOptionInstrCommRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryInstrumentOrderCommRate(
      CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryOptionInstrTradeCost(
      CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryOptionInstrCommRate(
      CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast);
  void OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
  void OnRspQryQuote(CThostFtdcQuoteField *pQuote,
                     CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                     bool bIsLast);
  void OnRspQryCombInstrumentGuard(
      CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryCombAction(CThostFtdcCombActionField *pCombAction,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
  void OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
  void OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast);
  void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                  bool bIsLast);
  void OnRtnOrder(CThostFtdcOrderField *pOrder);
  void OnRtnTrade(CThostFtdcTradeField *pTrade);
  void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                           CThostFtdcRspInfoField *pRspInfo);
  void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                           CThostFtdcRspInfoField *pRspInfo);
  void
  OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);
  void OnRtnBulletin(CThostFtdcBulletinField *pBulletin);
  void OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo);
  void OnRtnErrorConditionalOrder(
      CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder);
  void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);
  void OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder,
                               CThostFtdcRspInfoField *pRspInfo);
  void OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction,
                               CThostFtdcRspInfoField *pRspInfo);
  void OnErrRtnForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote,
                              CThostFtdcRspInfoField *pRspInfo);
  void OnRtnQuote(CThostFtdcQuoteField *pQuote);
  void OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                           CThostFtdcRspInfoField *pRspInfo);
  void OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction,
                           CThostFtdcRspInfoField *pRspInfo);
  void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);
  void OnRtnCFMMCTradingAccountToken(
      CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken);
  void
  OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction,
                           CThostFtdcRspInfoField *pRspInfo);
  void OnRtnCombAction(CThostFtdcCombActionField *pCombAction);
  void
  OnErrRtnCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction,
                           CThostFtdcRspInfoField *pRspInfo);
  void OnRspQryContractBank(CThostFtdcContractBankField *pContractBank,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
  void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
  void OnRspQryParkedOrderAction(
      CThostFtdcParkedOrderActionField *pParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast);
  void OnRspQryBrokerTradingParams(
      CThostFtdcBrokerTradingParamsField *pBrokerTradingParams,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQryBrokerTradingAlgos(
      CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRspQueryCFMMCTradingAccountToken(
      CThostFtdcQueryCFMMCTradingAccountTokenField *
          pQueryCFMMCTradingAccountToken,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer);
  void OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer);
  void OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField *pRspRepeal);
  void OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField *pRspRepeal);
  void OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer);
  void OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer);
  void OnRtnRepealFromBankToFutureByFutureManual(
      CThostFtdcRspRepealField *pRspRepeal);
  void OnRtnRepealFromFutureToBankByFutureManual(
      CThostFtdcRspRepealField *pRspRepeal);
  void OnRtnQueryBankBalanceByFuture(
      CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount);
  void OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer,
                                    CThostFtdcRspInfoField *pRspInfo);
  void OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer,
                                    CThostFtdcRspInfoField *pRspInfo);
  void
  OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal,
                                           CThostFtdcRspInfoField *pRspInfo);
  void
  OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal,
                                           CThostFtdcRspInfoField *pRspInfo);
  void OnErrRtnQueryBankBalanceByFuture(
      CThostFtdcReqQueryAccountField *pReqQueryAccount,
      CThostFtdcRspInfoField *pRspInfo);
  void
  OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal);
  void
  OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal);
  void OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer,
                                     CThostFtdcRspInfoField *pRspInfo,
                                     int nRequestID, bool bIsLast);
  void OnRspFromFutureToBankByFuture( CThostFtdcReqTransferField *pReqTransfer,
                                     CThostFtdcRspInfoField *pRspInfo,
                                     int nRequestID, bool bIsLast);
  void OnRspQueryBankAccountMoneyByFuture(
      CThostFtdcReqQueryAccountField *pReqQueryAccount,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
  void OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount);
  void OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount);
  void OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount);


private:
  VecInstrument vecinst_;
};

typedef int (*on_order_return)(CTPORDER*);

class CTPIF_API CtpTestTradeSpi : public CThostFtdcTraderSpi {
public:
    CtpTestTradeSpi(CtpSpeedParam *param, const char *logpath);
    ~CtpTestTradeSpi();

    void set_order_return(on_order_return oret) {oret_ = oret;}

    int run();
    int stop();
    int login();
    int logout();
    int prepare_environment();
    void set_session(CThostFtdcRspUserLoginField *pUserLogin);
    int confirm_settlement_info();
    int query_instrument(const char *instrument);
    int query_account();
    int query_investor_position(const char *instrument);
    int query_order(const char *instrument, const char *exchange,
                                 const char *ordersysid, const char *begintime,
                                 const char *endtime);
    int query_trade(const char *instrument, const char *exchange,
                                 const char *tradeid, const char *begintime,
                                 const char *endtime);
    int order_insert_limit_price(
        const char *instrument, char cDirection, double fPrice, int nVolume,
        char cVolumeCondition, const char *strGTDdate, char cTimeCondition,
        int nMinVolume, char cTrigerCondition, double fStopPrice);
    int order_insert_limitd(const char *instrument, char cDirection,
                                         double fPrice, int nVolume);
    int order_insert_condtn(const char *instrument, char cDirection,
                                         double fPrice, int nVolume,
                                         double fConditionPrice,
                                         char cTrigerCondition,
                                         char cPriceCondition);
    int order_corver_condtn(const char *instrument, char cDirection,
                                         double fPrice, int nVolume,
                                         double fConditionPrice,
                                         char cTrigerCondition,
                                         char cPriceCondition);
    int order_insert_parked();
    int order_corver(const char *instrument, char cDirection,
                                  double fPrice, int nVolume);
    int order_recall(CTPORDER *pOrder);
    char *GetOrderStatus(char status, char *buf, int len);
    int get_order_ref() {return SessionInfo_.MaxOrderRef;}

protected:
    void OnFrontConnected();
    void OnFrontDisconnected(int nReason);
    void OnHeartBeatWarning(int nTimeLapse);
    void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
    void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast);
    void OnRspUserPasswordUpdate(
        CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspTradingAccountPasswordUpdate(
        CThostFtdcTradingAccountPasswordUpdateField *
            pTradingAccountPasswordUpdate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                bool bIsLast);
    void
    OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
    void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspQueryMaxOrderVolume(
        CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspSettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void
    OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
    void OnRspRemoveParkedOrderAction(
        CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
    void OnRspExecOrderAction(
        CThostFtdcInputExecOrderActionField *pInputExecOrderAction,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast);
    void OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspBatchOrderAction(
        CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast);
    void OnRspQryOrder(CThostFtdcOrderField *pOrder,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast);
    void OnRspQryTrade(CThostFtdcTradeField *pTrade,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast);
    void
    OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast);
    void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                bool bIsLast);
    void OnRspQryInvestor(CThostFtdcInvestorField *pInvestor,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast);
    void OnRspQryInstrumentMarginRate(
        CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryInstrumentCommissionRate(
        CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryExchange(CThostFtdcExchangeField *pExchange,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspQryProduct(CThostFtdcProductField *pProduct,
                         CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                         bool bIsLast);
    void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
    void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData,
                                 CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                 bool bIsLast);
    void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                bool bIsLast);
    void OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
    void OnRspQryInvestorPositionDetail(
        CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryNotice(CThostFtdcNoticeField *pNotice,
                        CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast);
    void OnRspQrySettlementInfoConfirm(
        CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryInvestorPositionCombineDetail(
        CThostFtdcInvestorPositionCombineDetailField *
            pInvestorPositionCombineDetail,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryCFMMCTradingAccountKey(
        CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                bool bIsLast);
    void OnRspQryInvestorProductGroupMargin(
        CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryExchangeMarginRate(
        CThostFtdcExchangeMarginRateField *pExchangeMarginRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryExchangeMarginRateAdjust(
        CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
    void OnRspQrySecAgentACIDMap(CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap,
                                 CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                 bool bIsLast);
    void OnRspQryProductExchRate(CThostFtdcProductExchRateField *pProductExchRate,
                                 CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                 bool bIsLast);
    void OnRspQryProductGroup(CThostFtdcProductGroupField *pProductGroup,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
    void OnRspQryMMInstrumentCommissionRate(
        CThostFtdcMMInstrumentCommissionRateField *pMMInstrumentCommissionRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryMMOptionInstrCommRate(
        CThostFtdcMMOptionInstrCommRateField *pMMOptionInstrCommRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryInstrumentOrderCommRate(
        CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryOptionInstrTradeCost(
        CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryOptionInstrCommRate(
        CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast);
    void OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote,
                          CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);
    void OnRspQryQuote(CThostFtdcQuoteField *pQuote,
                       CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast);
    void OnRspQryCombInstrumentGuard(
        CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryCombAction(CThostFtdcCombActionField *pCombAction,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast);
    void OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                bool bIsLast);
    void OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister,
                                 CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                 bool bIsLast);
    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                    bool bIsLast);
    void OnRtnOrder(CThostFtdcOrderField *pOrder);
    void OnRtnTrade(CThostFtdcTradeField *pTrade);
    void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                             CThostFtdcRspInfoField *pRspInfo);
    void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                             CThostFtdcRspInfoField *pRspInfo);
    void
    OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus);
    void OnRtnBulletin(CThostFtdcBulletinField *pBulletin);
    void OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo);
    void OnRtnErrorConditionalOrder(
        CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder);
    void OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder);
    void OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder,
                                 CThostFtdcRspInfoField *pRspInfo);
    void OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction,
                                 CThostFtdcRspInfoField *pRspInfo);
    void OnErrRtnForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote,
                                CThostFtdcRspInfoField *pRspInfo);
    void OnRtnQuote(CThostFtdcQuoteField *pQuote);
    void OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote,
                             CThostFtdcRspInfoField *pRspInfo);
    void OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction,
                             CThostFtdcRspInfoField *pRspInfo);
    void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);
    void OnRtnCFMMCTradingAccountToken(
        CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken);
    void
    OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction,
                             CThostFtdcRspInfoField *pRspInfo);
    void OnRtnCombAction(CThostFtdcCombActionField *pCombAction);
    void
    OnErrRtnCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction,
                             CThostFtdcRspInfoField *pRspInfo);
    void OnRspQryContractBank(CThostFtdcContractBankField *pContractBank,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);
    void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast);
    void OnRspQryParkedOrderAction(
        CThostFtdcParkedOrderActionField *pParkedOrderAction,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast);
    void OnRspQryBrokerTradingParams(
        CThostFtdcBrokerTradingParamsField *pBrokerTradingParams,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQryBrokerTradingAlgos(
        CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRspQueryCFMMCTradingAccountToken(
        CThostFtdcQueryCFMMCTradingAccountTokenField *
            pQueryCFMMCTradingAccountToken,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer);
    void OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer);
    void OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField *pRspRepeal);
    void OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField *pRspRepeal);
    void OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer);
    void OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer);
    void OnRtnRepealFromBankToFutureByFutureManual(
        CThostFtdcRspRepealField *pRspRepeal);
    void OnRtnRepealFromFutureToBankByFutureManual(
        CThostFtdcRspRepealField *pRspRepeal);
    void OnRtnQueryBankBalanceByFuture(
        CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount);
    void OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer,
                                      CThostFtdcRspInfoField *pRspInfo);
    void OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer,
                                      CThostFtdcRspInfoField *pRspInfo);
    void
    OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal,
                                             CThostFtdcRspInfoField *pRspInfo);
    void
    OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal,
                                             CThostFtdcRspInfoField *pRspInfo);
    void OnErrRtnQueryBankBalanceByFuture(
        CThostFtdcReqQueryAccountField *pReqQueryAccount,
        CThostFtdcRspInfoField *pRspInfo);
    void
    OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal);
    void
    OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal);
    void OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer,
                                       CThostFtdcRspInfoField *pRspInfo,
                                       int nRequestID, bool bIsLast);
    void OnRspFromFutureToBankByFuture( CThostFtdcReqTransferField *pReqTransfer,
                                       CThostFtdcRspInfoField *pRspInfo,
                                       int nRequestID, bool bIsLast);
    void OnRspQueryBankAccountMoneyByFuture(
        CThostFtdcReqQueryAccountField *pReqQueryAccount,
        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    void OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount);
    void OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount);
    void OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount);


private:
    CtpSpeedParam *param_ = nullptr;
    CThostFtdcTraderApi *ctp_ = nullptr;
    int nRequestID_ = 0;
    TdifSessionInfo SessionInfo_;
    char logpath_[MAX_PATH] = {""};


    on_order_return oret_ = nullptr;
};

// extern CTPIF_API CtpTradeSpi *gtrade;


#endif // CTP_TRADE_API_SPI_H
