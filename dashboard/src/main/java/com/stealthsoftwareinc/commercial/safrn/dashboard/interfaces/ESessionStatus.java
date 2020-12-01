package com.stealthsoftwareinc.commercial.safrn.dashboard.interfaces;

public enum ESessionStatus {//overall session status
  CREATED,//initiated
  ACCEPTED,//all participants accepted
  DECLINED,
  ACTIVE,//every participant downloaded the config files
  INITIATED,
  DRAFT,
  EXPIRED; //not sure
}
