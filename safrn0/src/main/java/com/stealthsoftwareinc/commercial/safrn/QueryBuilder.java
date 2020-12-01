/*
 * This file is from the SAFRN package.
 *
 * The following copyright notice is generally applicable:
 *
 *      Copyright (C)
 *         Stealth Software Technologies Commercial, Inc.
 *
 * The full copyright information depends on the distribution
 * of the package. For more information, see the COPYING file.
 * However, depending on the context in which you are viewing
 * this file, the COPYING file may not be available.
 */

/*!
 * @file
 *
 * Defines the
 * <code>com.stealthsoftwareinc.commercial.safrn.QueryBuilder</code>
 * Java class.
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

package com.stealthsoftwareinc.commercial.safrn;

/* begin_imports */

import com.stealthsoftwareinc.sst.JdbcSubprotocol;
import com.stealthsoftwareinc.sst.NullPointerStatus;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Formatter;
import java.lang.StringBuilder;

/* end_imports */

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public class QueryBuilder
{

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private String analysis;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final Map<String, ArrayList<String>> domainNames;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final List<String> dvList;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final List<String> ivList;

/**
 *
 * @since SAFRN&nbsp;X.X.X (XXXX-XX-XX)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final boolean loanlinkbool;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private int localParty;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private final StringBuilder queryBuilder;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private int remoteParty;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private String tableName;

private final JdbcSubprotocol subprotocol;

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public QueryBuilder(
  final JdbcSubprotocol subprotocol
) {
  this.analysis = null;
  this.loanlinkbool = false;
  this.domainNames = new HashMap<String, ArrayList<String>>();
  this.dvList = new ArrayList<String>();
  this.ivList = new ArrayList<String>();
  this.localParty = 0;
  this.queryBuilder = new StringBuilder();
  this.tableName = null;
  this.subprotocol = subprotocol;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public QueryBuilder(
  String analysis,
  boolean loanlinkbool,
  Map<String, ArrayList<String>> domainNames,
  List<String> dvList,
  List<String> ivList,
  int localParty,
  int remoteParty,
  String tableName,
  final JdbcSubprotocol subprotocol
) {
  if (dvList == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "dvList is a null reference"
      ).initCause(null)
    ;
  }
  if (ivList == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "ivList is a null reference"
      ).initCause(null)
    ;
  }
  if (domainNames == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "domainNames is a null reference"
      ).initCause(null)
    ;
  }
  this.analysis = analysis;
  this.loanlinkbool = loanlinkbool;
  this.domainNames = domainNames;
  this.dvList = dvList;
  this.ivList = ivList;
  this.localParty = localParty;
  this.remoteParty = remoteParty;
  this.queryBuilder = new StringBuilder();
  this.tableName = tableName;
  this.subprotocol = subprotocol;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private static String mEscape(
  String in
) {
  return in; // TODO: Escape
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public void setAnalysis(
  String analysis
) {
  this.analysis = analysis;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public void setDvList(
  List<String> dvList
) throws
  Exception
{
  if (dvList == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "dvList is a null reference"
      ).initCause(null)
    ;
  }
  this.dvList.clear();
  this.dvList.addAll(dvList);
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public void setIvList(
  List<String> ivList
) throws
  Exception
{
  if (ivList == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "ivList is a null reference"
      ).initCause(null)
    ;
  }
  this.ivList.clear();
  this.ivList.addAll(ivList);
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public void setLocalParty(
  int localParty
) {
  this.localParty = localParty;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public void setRemoteParty(
  int remoteParty
) {
  this.remoteParty = remoteParty;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public void setTableName(
  String tableName
) {
  this.tableName = tableName;
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

public String buildQuery(
) throws
  Exception
{
  if (this.analysis == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "analysis is a null reference"
      ).initCause(null)
    ;
  }
  if (this.tableName == null) {
    throw (NullPointerStatus)
      new NullPointerStatus(
        "tableName is a null reference"
      ).initCause(null)
    ;
  }

  if (this.analysis.equals("freq")) {
    if (this.localParty == 0) {
      return null;
    } else if (this.localParty == 1) {
      if (this.ivList.size() == 0) {
        return "SELECT COUNT(*) FROM " + mEscape(this.tableName);
      } else {
        return null;
      }
    } else {
      if (this.ivList.size() == 0) {
        return null;
      } else {
        return handleGroupFreq();
      }
    }
  } else { /*handle means*/
    if (this.localParty == 0) {
      return null;
    } else if (this.localParty == 1) {
      if (this.ivList.size() == 0) {
        if (this.dvList.size() == 0) {
          return null;
        }
        queryBuilder.setLength(0);
        queryBuilder.append(
          "SELECT\n"
        );
        for (int i = 0; i != dvList.size(); ++i) {
          queryBuilder.append(String.format("  SUM(%s) AS %s",mEscape(dvList.get(i)),mEscape(dvList.get(i))));
            queryBuilder.append(",");
          queryBuilder.append("\n");
        }
        queryBuilder.append("  COUNT(*) AS count\n");
        queryBuilder.append("FROM ");
        queryBuilder.append(mEscape(this.tableName));
        return queryBuilder.toString();
      } else {
        queryBuilder.setLength(0);
        queryBuilder.append(
          "SELECT\n  CaseID,\n"
        );
        for (int i = 0; i != dvList.size(); ++i) {
          queryBuilder.append(String.format("  %s",mEscape(dvList.get(i))));
          if (i < dvList.size() - 1) { queryBuilder.append(","); }
          queryBuilder.append("\n");
        }
        queryBuilder.append("FROM ");
        queryBuilder.append(mEscape(this.tableName));
        if (this.loanlinkbool) {
          queryBuilder.append(" WHERE ");
          queryBuilder.append(mEscape(this.tableName));
          queryBuilder.append(".School = '");
          queryBuilder.append(mEscape(this.domainNames.get("School").get(this.remoteParty - 2)));
          queryBuilder.append("'");
        }
        return queryBuilder.toString();
      }
    } else {
      if (this.ivList.size() == 0) {
        return null;
      } else {
        return handleGroupMean();
      }
    }  }

}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private String handleGroupFreq(
) {
  queryBuilder.setLength(0);
  queryBuilder.append(
    "SELECT\n"
  );

  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("%s,\n",mEscape(this.ivList.get(i)))
    );
  }

  queryBuilder.append(
    String.format("(\n  SELECT COUNT(*) FROM\n  %s\n  WHERE\n  (\n",mEscape(this.tableName))
  );

  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format(
        "    ((%s.%s=%s_domain.%s) OR (%s_domain.%s='\"*\"'))",
        mEscape(this.tableName),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i))
      )
    );
    if (i != this.ivList.size() - 1) {
      queryBuilder.append(
        " AND"
      );
    }
    queryBuilder.append(
      "\n"
    );
  }
  queryBuilder.append("  )\n)\nAS freq FROM\n(\n");
  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("  %s_domain", mEscape(this.ivList.get(i)))
    );
    if (i != this.ivList.size() - 1) {
      queryBuilder.append(
        " CROSS JOIN"
      );
    }
    queryBuilder.append(
      "\n"
    );
  }
  queryBuilder.append(")\nORDER BY\n");
  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("  %s_domain.id", mEscape(this.ivList.get(i)), mEscape(this.ivList.get(i)))
    );
    if (i != this.ivList.size() - 1) {
      queryBuilder.append(
        ","
      );
    }
    queryBuilder.append(
      "\n"
    );
  }
  return queryBuilder.toString();
}

/**
 *
 * @since SAFRN&nbsp;0.1.0 (2017-11-08)
 *
 * @examples
 * none
 *
 * @notablechanges
 * none
 */

private String handleGroupMean(
) {
  queryBuilder.setLength(0);
  queryBuilder.append(
    "SELECT\n"
  );

  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("%s_domain.id AS %s_orderfix",mEscape(this.ivList.get(i)),mEscape(this.ivList.get(i)),mEscape(this.ivList.get(i)))
    );
    if (true) { queryBuilder.append(","); }
    queryBuilder.append("\n");
  }

  queryBuilder.append("  CaseID\n  FROM\n(\n");

  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("  %s_domain CROSS JOIN\n", mEscape(this.ivList.get(i)))
    );
  }
  queryBuilder.append(String.format("  %s\n) WHERE\n", mEscape(this.tableName)));


  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("  ((%s.%s=%s_domain.%s) OR (%s_domain.%s='\"*\"'))",
        mEscape(this.tableName),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i)),
        mEscape(this.ivList.get(i))
      )
    );
    if (i != this.ivList.size() - 1) {
      queryBuilder.append(" AND");
    }
    queryBuilder.append("\n");
  }
  queryBuilder.append(
    "UNION\nSELECT\n"
  );
  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("%s_domain.id AS %s_orderfix",mEscape(this.ivList.get(i)),mEscape(this.ivList.get(i)),mEscape(this.ivList.get(i)))
    );
    if (true) { queryBuilder.append(","); }
    queryBuilder.append("\n");
  }
  queryBuilder.append("  NULL AS CaseID\n  FROM\n(\n");
  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("  %s_domain", mEscape(this.ivList.get(i))));
          if (i < this.ivList.size() - 1) { queryBuilder.append(" CROSS JOIN"); }
    queryBuilder.append("\n");
  }
  queryBuilder.append(String.format(")\n"));
  queryBuilder.append("ORDER BY\n");
  for (int i = 0; i != this.ivList.size(); ++i) {
    queryBuilder.append(
      String.format("  %s_orderfix,\n", mEscape(this.ivList.get(i)))
    );
  }
  queryBuilder.append(
    "  CaseID\n"
  );

  if (this.subprotocol.equals(JdbcSubprotocol.ORACLE)) {
    queryBuilder.append(" NULLS FIRST");
  }

  return queryBuilder.toString();
}
}
