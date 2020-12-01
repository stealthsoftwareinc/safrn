import React, { createContext, useReducer } from 'react'

// Reducer types
export const UIR_CUR_BLOCK = 'curBlock'
export const UIR_CUR_BLOCK_PAGE = 'curBlockPage'
export const UIR_START_BLOCK_PAGE = 'startBlockPage'
export const UIR_BLOCK_LIST = 'blockList'
export const UIR_BLOCK_PAGE_SIZE = 'blockPageSize'
export const UIR_CUR_HOUR_ITEM = 'curHourItem'
export const UIR_CUR_HOUR_ITEM_PAGE = 'curHourItemPage'
export const UIR_START_HOUR_ITEM_PAGE = 'startHourItemPage'
export const UIR_HOUR_ITEM_LIST = 'HourItemList'
export const UIR_HOUR_ITEM_PAGE_SIZE = 'HourItemPageSize'
export const UIR_CUR_DAY_ITEM = 'curDayItem'
export const UIR_CUR_DAY_ITEM_PAGE = 'curDayItemPage'
export const UIR_START_DAY_ITEM_PAGE = 'startDayItemPage'
export const UIR_DAY_ITEM_LIST = 'DayItemList'
export const UIR_DAY_ITEM_PAGE_SIZE = 'DayItemPageSize'
export const UIR_CUR_MONTH_ITEM = 'curMonthItem'
export const UIR_CUR_MONTH_ITEM_PAGE = 'curMonthItemPage'
export const UIR_START_MONTH_ITEM_PAGE = 'startMonthItemPage'
export const UIR_MONTH_ITEM_LIST = 'MonthItemList'
export const UIR_MONTH_ITEM_PAGE_SIZE = 'MonthItemPageSize'
export const UIR_INVOICE_PAGE_SIZE = 'InvoicePageSize'
export const UIR_CUR_INVOICE_PAGE = 'curInvoicePage'
export const UIR_START_INVOICE_PAGE = 'startInvoicePage'
export const UIR_DECREMENT_PAGER = 'decrementPager'
export const UIR_INCREMENT_PAGER = 'incrementPager'
export const UIR_FILTER = 'filter'
export const UIR_FILTERON = 'filterOn'

// Filter fields
export const FILTER_ITEM_TYPE = 'filterItemType'
export const FILTER_FROM_DATE = 'filterFromDate'
export const FILTER_FROM_TIME = 'filterFromTime'
export const FILTER_TO_DATE = 'filterToDate'
export const FILTER_TO_TIME = 'filterToTime'
export const FILTER_NUM_INVOICES = 'filterNumInvoices'
export const FILTER_AMOUNT = 'filterAmount'
export const FILTER_INV_STATE = 'filterInvState'
export const FILTER_VENDOR = 'filterVendor'
export const FILTER_BUYER = 'filterBuyer'
export const FILTER_VENDER_BUYER_AND = 'filterVendorBuyerAND'

// Filter Item type constants.
export const ITEM_TYPE_BLOCK = 'block'
export const ITEM_TYPE_INVOICE = 'invoice'

// Filter number internal fields.
export const NUM_SIGN = 'numSign'
export const NUM_VAL = 'numVal'
export const NUM_SIGN_GT = 'gt'
export const NUM_SIGN_EQ = 'eq'
export const NUM_SIGN_LT = 'lt'

// Select field "all" state.
export const FILTER_ALL = 'all'

// Invoice Filter state constants.
export const INV_STATE_CREATION = 'creation'
export const INV_STATE_PROPOSED = 'proposed'
export const INV_STATE_FULFILLED = 'fulfilled'
export const INV_STATE_CANCELLED = 'cancelled'

export const getEmptyFilter = () => {
  return {
    [FILTER_ITEM_TYPE]: ITEM_TYPE_BLOCK,
    [FILTER_FROM_DATE]: '',
    [FILTER_FROM_TIME]: '',
    [FILTER_TO_DATE]: '',
    [FILTER_TO_TIME]: '',
    [FILTER_AMOUNT]: { [NUM_SIGN]: NUM_SIGN_EQ, [NUM_VAL]: '' },
    [FILTER_NUM_INVOICES]: { [NUM_SIGN]: NUM_SIGN_EQ, [NUM_VAL]: '' },
    [FILTER_VENDOR]: FILTER_ALL,
    [FILTER_BUYER]: FILTER_ALL,
    [FILTER_VENDER_BUYER_AND]: true,
  }
}

const initialState = {
  [UIR_CUR_BLOCK]: 0,
  [UIR_CUR_BLOCK_PAGE]: 0,
  [UIR_START_BLOCK_PAGE]: 1,
  [UIR_BLOCK_LIST]: [],
  [UIR_BLOCK_PAGE_SIZE]: 0,
  [UIR_CUR_HOUR_ITEM]: 0,
  [UIR_CUR_HOUR_ITEM_PAGE]: 0,
  [UIR_START_HOUR_ITEM_PAGE]: 1,
  [UIR_HOUR_ITEM_LIST]: [],
  [UIR_HOUR_ITEM_PAGE_SIZE]: 0,
  [UIR_CUR_DAY_ITEM]: 0,
  [UIR_CUR_DAY_ITEM_PAGE]: 0,
  [UIR_START_DAY_ITEM_PAGE]: 1,
  [UIR_DAY_ITEM_LIST]: [],
  [UIR_DAY_ITEM_PAGE_SIZE]: 0,
  [UIR_CUR_MONTH_ITEM]: 0,
  [UIR_CUR_MONTH_ITEM_PAGE]: 0,
  [UIR_START_MONTH_ITEM_PAGE]: 1,
  [UIR_MONTH_ITEM_LIST]: [],
  [UIR_MONTH_ITEM_PAGE_SIZE]: 0,
  [UIR_INVOICE_PAGE_SIZE]: 0,
  [UIR_CUR_INVOICE_PAGE]: 0,
  [UIR_START_INVOICE_PAGE]: 1,
  [UIR_DECREMENT_PAGER]: {
    curPage: UIR_CUR_BLOCK_PAGE,
    startPage: UIR_START_BLOCK_PAGE,
  },
  [UIR_INCREMENT_PAGER]: {
    curPage: UIR_CUR_BLOCK_PAGE,
    startPage: UIR_START_BLOCK_PAGE,
    pageSize: UIR_BLOCK_PAGE_SIZE,
  },
  [UIR_FILTER]: getEmptyFilter(),
  // Make this a number to indicate changes when the filter button is pressed
  [UIR_FILTERON]: 0,
}

export const uiReducer = (state, action) => {
  console.log('action type = ' + action.type)
  console.log(action.payload)
  switch (action.type) {
    case UIR_FILTERON:
      return {
        ...state,
        [UIR_FILTERON]: action.payload,
        [UIR_CUR_BLOCK]: 0,
        [UIR_CUR_BLOCK_PAGE]: 0,
        [UIR_START_BLOCK_PAGE]: 1,
        [UIR_CUR_HOUR_ITEM]: 0,
        [UIR_CUR_HOUR_ITEM_PAGE]: 0,
        [UIR_START_HOUR_ITEM_PAGE]: 1,
        [UIR_CUR_DAY_ITEM]: 0,
        [UIR_CUR_DAY_ITEM_PAGE]: 0,
        [UIR_START_DAY_ITEM_PAGE]: 1,
        [UIR_CUR_MONTH_ITEM]: 0,
        [UIR_CUR_MONTH_ITEM_PAGE]: 0,
        [UIR_START_MONTH_ITEM_PAGE]: 1,
      }
    case UIR_DECREMENT_PAGER:
      let decrPage = state[action.payload.startPage] - 3
      if (decrPage < 1) {
        decrPage = 1
      }
      return {
        ...state,
        [action.payload.startPage]: decrPage,
      }
    case UIR_INCREMENT_PAGER:
      let incrPage =
        state[action.payload.startPage] >= state[action.payload.pageSize] - 6
          ? state[action.payload.pageSize] - 4
          : state[action.payload.startPage] + 3
      return {
        ...state,
        [action.payload.startPage]: incrPage,
      }
    default:
      return {
        ...state,
        [action.type]: action.payload,
      }
  }
}

export const UIStore = ({ children }) => {
  const [state, dispatch] = useReducer(uiReducer, initialState)
  return (
    <Context.Provider value={[state, dispatch]}>{children}</Context.Provider>
  )
}

export const Context = createContext(initialState)
