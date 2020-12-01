import React from 'react';
import './analystView.scss';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faPlus, faTimesCircle } from '@fortawesome/free-solid-svg-icons';
//import { Context } from '../../store/UIStore';
import { getLexicon } from '../../utils/Api';

function AnalystView() {
  //const [state, dispatch] = useContext(Context);
  const [lexicon, setLexicon] = React.useState([]);
  const [fields, setFields] = React.useState({});
  const [verticals, setVerticals] = React.useState([]);
  const [showQueryJson, setShowQueryJson] = React.useState(false)
  const emptyQuery = {
    preFilters: [],
    joins: [],
    postFilters: [],
    queryFunction: [],
  };
  const OrderFunction = "OrderFunction"
  const MomentFunction = "MomentFunction"
  const LinearRegressionFunction = "LinearRegressionFunction"
  const FTestFunction = "FTestFunction"
  const TTestFunction = "TTestFunction"

  const [query, setQuery] = React.useState(emptyQuery);

  const getColsForVertical = (vert) => {
    const vertIndex = lexicon.findIndex(l => l.verticalId === vert)
    return [...fields[vertIndex]]
  }

  const setEmptyMultnomialValue = (mv, vert) => {
    mv.push( {
      col: { vertical: vert, column: lexicon[0].columns[0].name },
      exp: 1,
    } )
  }

  const setEmptyMultiNomial = (mnom, vert) => {
    mnom.push({ coefficient: 1, values: [] })
    setEmptyMultnomialValue(mnom[mnom.length - 1].values, vert)
  }

  const setEmptyAnd = (andObj, vert) => {
    andObj.push({ left: [], right: 1, comp: "=" })
    setEmptyMultiNomial(andObj[andObj.length - 1].left, vert)
  }

  const setEmptyOr = (orObj, vert) => {
    orObj.push([])
    setEmptyAnd(orObj[orObj.length - 1], vert)
  }

  const setEmptyVert = (vert) => {
    const newVert = []
    setEmptyOr(newVert, vert)
    return newVert
  }

  const clearRules = () => {
    setVerticals([])
    let empty = { ...emptyQuery }
    empty.queryFunction = getInitialQueryFunctOrder()
    setQuery( empty )
  };

  const getUnusedVerticalsList = (curIndex = -1) => {
    let vertNames = []

    lexicon.forEach(v => {
      if (verticals.indexOf(v.verticalId) < 0 ||
        (curIndex >= 0 && verticals[curIndex] === v.verticalId)) {
        vertNames.push(v.verticalId)
      }
    })

    return vertNames
  }

  const findFirstOpenVertical = () => {
    const unusedVerticals = getUnusedVerticalsList()

    if (unusedVerticals.length > 0) {
      return unusedVerticals[0]
    }
    
    return null
  }

  const addVertical = () => {
    let preFilters = query.preFilters
    const vert = findFirstOpenVertical()
    setVerticals([...verticals, vert])

    preFilters.push(setEmptyVert(vert))
    setQuery({
      ...query,
      preFilters: preFilters,
    });
  };

  const deleteVertical = (index) => {
    const newFilter = query.preFilters.filter((f, i) => i !== index)
    const newVerticals = verticals.filter((v, i) => i !== index)
    setVerticals(newVerticals)
    setQuery({ ...query, preFilters: newFilter })
  };

  const deleteOr = (pfIndex, orIndex) => {
    const newFilter = query.preFilters
    newFilter[pfIndex].splice(orIndex, 1)
    setQuery({ ...query, preFilters: newFilter })
  }

  const onChangeVertical = (index, vert) => {
    const verts = [ ...verticals ];
    verts[index] = vert;
    setVerticals(verts);
    let preFilters = query.preFilters
    preFilters[index] = setEmptyVert(vert) // clear vertical.
    setQuery({
      ...query,
      preFilters: preFilters,
    });
  };

  const addOrClause = (index) => {
    let preFilters = query.preFilters
    setEmptyOr(preFilters[index])
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const addAndClause = (pfIndex, orIndex) => {
    let preFilters = query.preFilters
    setEmptyAnd(preFilters[pfIndex][orIndex], verticals[pfIndex])
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const addMultinomial = (pfIndex, orIndex, andIndex) => {
    let preFilters = query.preFilters
    // New multinomial row.
    setEmptyMultiNomial(preFilters[pfIndex][orIndex][andIndex].left, verticals[pfIndex])
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const addMultinomialValue = (pfIndex, orIndex, andIndex, mnIndex) => {
    let preFilters = query.preFilters
    // New multinomial row.
    setEmptyMultnomialValue(preFilters[pfIndex][orIndex][andIndex].left[mnIndex].values,
      verticals[pfIndex])

    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const addJoin = () => {
    let joins = query.joins
    joins.push(
      {
        type: "INNER",
        joinOns: [
          {
            first: {
              col: {
                vertical: lexicon[0].verticalId,
                column: fields[0][0]
              },
              formula: [0, 1]
            },
            second: {
              col: {
                vertical: lexicon[1].verticalId,
                column: fields[1][0]
              },
              formula: [0, 1]
            },
          }
        ]
      }
    )
    setQuery({
      ...query,
      joins: joins,
    })

  }

  const updateComp = (pfIndex, orIndex, andIndex, newVal) => {
    let preFilters = query.preFilters
    preFilters[pfIndex][orIndex][andIndex].comp = newVal
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const updateRight = (pfIndex, orIndex, andIndex, newVal) => {
    let preFilters = query.preFilters
    preFilters[pfIndex][orIndex][andIndex].right = newVal
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const updateCoeffient = (pfIndex, orIndex, andIndex, mnIndex, newVal) => {
    let preFilters = query.preFilters
    preFilters[pfIndex][orIndex][andIndex].left[mnIndex].coefficient = newVal
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const updateCol = (pfIndex, orIndex, andIndex, mnIndex, mnVal, newVal) => {
    let preFilters = query.preFilters
    preFilters[pfIndex][orIndex][andIndex].left[mnIndex].values[mnVal].col.column = newVal
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const updateExp = (pfIndex, orIndex, andIndex, mnIndex, mnVal, newVal) => {
    let preFilters = query.preFilters
    preFilters[pfIndex][orIndex][andIndex].left[mnIndex].values[mnVal].exp = newVal
    setQuery({
      ...query,
      preFilters: preFilters,
    })
  }

  const updateJoinType = (joinIndex, newVal) => {
    let joins = query.joins
    joins[joinIndex].type = newVal
    setQuery({
      ...query,
      joins: joins,
    })
  }

  const updateJoinVertical = (joinIndex, leftOrRight, newVal) => {
    let joins = query.joins
    joins[joinIndex].joinOns[0][leftOrRight].col.vertical = newVal

    let colNames = getColsForVertical(newVal)
    joins[joinIndex].joinOns[0][leftOrRight].col.column = colNames[0]
    setQuery({
      ...query,
      joins: joins,
    })
  }

  const updateJoinColumn = (joinIndex, leftOrRight, newVal) => {
    let joins = query.joins
    joins[joinIndex].joinOns[0][leftOrRight].col.column = newVal
    setQuery({
      ...query,
      joins: joins,
    })
  }

  const getMultiNomialValueDisplay =
    (mnObj, pfIndex, orIndex, andIndex, mnIndex, valIndex) => {

    let colNames = [...fields[pfIndex]]
    return (
      <div key={'mnval' + pfIndex + '-' + orIndex + '-' + andIndex + '-' +
        mnIndex + '-' + valIndex}
        className="mnval-col">
        <div>
          <div>Column</div>
          <select
            value={mnObj.col.column}
            onChange={e => updateCol(pfIndex, orIndex, andIndex, mnIndex, valIndex, e.currentTarget.value)}
          >
            {colNames.map(c => (
              <option value={c} key={'col' + pfIndex + c}>{c}</option>
            ))}
          </select>
        </div>
        <div>
          <div>EXP</div>
          <input
            type="number"
            value={mnObj.exp}
            onChange={e => updateExp(pfIndex, orIndex, andIndex, mnIndex, valIndex, e.currentTarget.value)}
          ></input>
        </div>
      </div>
    )
  }

  const getMultiNomialDisplay = (mnObj, pfIndex, orIndex, andIndex, mnIndex) => {
    return (
      <div key={'mn' + pfIndex + '-' + orIndex + '-' + andIndex + '-' + mnIndex}
        className="mn-col">
        <div className="coefficient">
          <div>Coefficient</div>
          <input type="number" value={mnObj.coefficient}
            onChange={e => updateCoeffient(pfIndex, orIndex, andIndex, mnIndex, e.currentTarget.value)}>
          </input>
        </div>
        <div>
          <FontAwesomeIcon
            className="add-icon add-left"
            icon={faPlus}
            size="lg"
            onClick={() => addMultinomialValue(pfIndex, orIndex, andIndex, mnIndex)}
          />
          <span className="left-spacing">Add Multiplicative Term</span>
          <div className="mnval-panel">
            {query.preFilters[pfIndex][orIndex][andIndex].left[mnIndex] &&
              query.preFilters[pfIndex][orIndex][andIndex].left[mnIndex].values.map((mnvalObj, i) =>
                getMultiNomialValueDisplay(mnvalObj, pfIndex, orIndex, andIndex, mnIndex, i)
              )}
          </div>
        </div>
      </div>
    )
  }

  const getAndDisplay = (andObj, pfIndex, orIndex, andIndex) => {
    return (
      <div key={'and' + pfIndex + '-' + orIndex + '-' + andIndex}
        className="and-col">
        <div>
          <FontAwesomeIcon
            className="add-icon add-left"
            icon={faPlus}
            size="lg"
              onClick={() => addMultinomial(pfIndex, orIndex, andIndex)}
            />
            <span className="left-spacing">Add Additive Term</span>
          {query.preFilters[pfIndex][orIndex][andIndex] &&
            query.preFilters[pfIndex][orIndex][andIndex].left.map((mnObj, i) =>
              getMultiNomialDisplay(mnObj, pfIndex, orIndex, andIndex, i)
            )
          }
        </div>
        <div>
          <div>
            <div>&nbsp;</div>
            <select value={andObj.comp}
              onChange={e =>
                updateComp(pfIndex, orIndex, andIndex, e.currentTarget.value)}>
              <option value="=">=</option>
              <option value="<">&lt;</option>
              <option value="<=">&lt;=</option>
              <option value=">">&gt;</option>
              <option value=">=">&gt;=</option>
              <option value="!=">!=</option>
            </select>
          </div>
        </div>
        <div>
          <div>
            <div>Right</div>
            <input type="number" value={andObj.right}
              onChange={e => updateRight(pfIndex, orIndex, andIndex, e.currentTarget.value)}>
            </input>
          </div>
        </div>
      </div>
    )
  }

  const getOrDisplay = (orObj, pfIndex, orIndex) => {
    return (
      <div key={'or' + pfIndex + '-' + orIndex} className="or-section">
        <div className="add-del-row">
          <div>
            <FontAwesomeIcon
              className="add-icon add-and"
              icon={faPlus}
              size="lg"
              onClick={() => addAndClause(pfIndex, orIndex)}
            />
            <span className="left-spacing">Add AND Clause</span>
          </div>
          <div>
            <FontAwesomeIcon
              className="del-icon"
              icon={faTimesCircle}
              size="lg"
              onClick={() => deleteOr(pfIndex, orIndex)}
            />
            <span className="left-spacing">Remove OR Clause</span>
          </div>
        </div>
        {query.preFilters[pfIndex][orIndex] &&
          query.preFilters[pfIndex][orIndex].map((andObj, i) =>
            getAndDisplay(andObj, pfIndex, orIndex, i)
          )
        }
      </div>
    )
  }

  const getVerticalsDisplay = (index) => {
    const verts = getUnusedVerticalsList(index)
    return (
      <div key={'vert' + index} className="vert-section">
        <div className="vert-row">
          <div className="label-select">
            <div>Choose Vertical</div>
            <select
              value={verticals[index]}
              onChange={(e) => onChangeVertical(index, e.currentTarget.value)}
            >
              {verts.map((li) => (
                <option value={li} key={'vert' + li}>
                  {li}
                </option>
              ))}
            </select>
          </div>
        </div>
        <div className="add-del-row">
          <div>
            <FontAwesomeIcon
              className="add-icon add-or"
              icon={faPlus}
              size="lg"
              onClick={() => addOrClause(index)}
            />
            <span className="left-spacing">Add OR Clause</span>
          </div>
          <div>
            <FontAwesomeIcon
              className="del-icon"
              icon={faTimesCircle}
              size="lg"
              onClick={() => deleteVertical(index)}
            />
            <span className="left-spacing">Remove Vertical</span>
          </div>
        </div>
        {query.preFilters[index] &&
          query.preFilters[index].map((pf, j) =>
            getOrDisplay(pf, index, j)
          )
        }
      </div>
    );
  };

  const getJoinsDisplay = (index) => {
    return (
      <div key={'join' + index} className="join-section">
        <div>
          <div>Choose Join Type</div>
          <select
            value={query.joins[index].type}
            onChange={(e) => updateJoinType(index, e.currentTarget.value)}
          >
            <option value="INNER">INNER</option>
            <option value="OUTER">OUTER</option>
            <option value="LEFT">LEFT</option>
            <option value="RIGHT">RIGHT</option>
          </select>
        </div>
        <div>
          <div>Choose Vertical 1</div>
          <select
            value={query.joins[index].joinOns[0].first.col.vertical}
            onChange={(e) => updateJoinVertical(index, "first", e.currentTarget.value)}
          >
            {lexicon.map((li) => (
              <option value={li.verticalId} key={'vert' + li.verticalId}>
                {li.verticalId}
              </option>
            ))}
          </select>
        </div>
        <div>
          <div>Choose Column 1</div>
          <select
            value={query.joins[index].joinOns[0].first.col.column}
            onChange={(e) => updateJoinColumn(index, "first", e.currentTarget.value)}
          >
            {getColsForVertical(query.joins[index].joinOns[0].first.col.vertical).map((cn) => (
              <option value={cn} key={'joinCol' + cn}>
                {cn}
              </option>
            ))}
          </select>
        </div>
        <div>
          <div>Choose Vertical 2</div>
          <select
            value={query.joins[index].joinOns[0].second.col.vertical}
            onChange={(e) => updateJoinVertical(index, "second", e.currentTarget.value)}
          >
            {lexicon.map((li) => (
              <option value={li.verticalId} key={'vert' + li.verticalId}>
                {li.verticalId}
              </option>
            ))}
          </select>
        </div>
        <div>
          <div>Choose Column 2</div>
          <select
            value={query.joins[index].joinOns[0].second.col.column}
            onChange={(e) => updateJoinColumn(index, "second", e.currentTarget.value)}
          >
            {getColsForVertical(query.joins[index].joinOns[0].second.col.vertical).map((cn) => (
              <option value={cn} key={'joinCol' + cn}>
                {cn}
              </option>
            ))}
          </select>
        </div>
      </div>
    )
  }

  const getInitialQueryFunctOrder = () => {
    if (lexicon.length > 0) {
      return {
        type: OrderFunction,
        col: { vertical: lexicon[0].verticalId, column: lexicon[0].columns[0].name },
        is_percentile: true,
        lowest_first: false,
        value: 0,
      }
    } else {
      return []
    }
  }

  const getInitialQueryFunctMoment = () => {
    return {
      type: MomentFunction,
      col: { vertical: lexicon[0].verticalId, column: lexicon[0].columns[0].name },
      momentType: "COUNT",
    }
  }

  const getInitialQueryFunctLinAndTest = (type) => {
    return {
      type: type,
      dep_var: { vertical: lexicon[0].verticalId, column: lexicon[0].columns[0].name },
      indep_vars: [],
    }
  }

  const setQueryFuncType = (ftype) => {
    let qfunct = query.queryFunction
    switch (ftype) {
      case OrderFunction: qfunct = getInitialQueryFunctOrder()
        break
      case MomentFunction: qfunct = getInitialQueryFunctMoment()
        break
      default: qfunct = getInitialQueryFunctLinAndTest(ftype)      
    }
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFColVertical = (vert) => {
    let qfunct = query.queryFunction
    qfunct.col.vertical = vert
    qfunct.col.column = getColsForVertical(vert)[0]
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFColColumn = (column) => {
    let qfunct = query.queryFunction
    qfunct.col.column = column
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFDepVarVertical = (vert) => {
    let qfunct = query.queryFunction
    qfunct.dep_var.vertical = vert
    qfunct.dep_var.column = getColsForVertical(vert)[0]
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFDepVarColumn = (column) => {
    let qfunct = query.queryFunction
    qfunct.dep_var.column = column
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFGenericValue = (key, val) => {
    let qfunct = query.queryFunction
    qfunct[key] = val
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const addIndependentVar = () => {
    let qfunct = query.queryFunction
    qfunct.indep_vars.push({
      vertical: lexicon[0].verticalId,
      column: getColsForVertical(lexicon[0].verticalId)[0]
    })
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFIndepVarVertical = (index, vert) => {
    let qfunct = query.queryFunction
    qfunct.indep_vars[index].vertical = vert
    qfunct.indep_vars[index].column = getColsForVertical(vert)[0]
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const updateQFIndepVarColumn = (index, column) => {
    let qfunct = query.queryFunction
    qfunct.indep_vars[index].column = column
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  const getQueryFunctionDisplay = () => {
    return (
      <div className={query.queryFunction.type === OrderFunction ? "qf-order-row" :
        query.queryFunction.type === MomentFunction? "qf-moment-row": "qf-generic-row"}>
        <div>
          <div>Choose Function Type</div>
          <select value={query.queryFunction.type}
            onChange={(e) => setQueryFuncType(e.currentTarget.value)}>
            <option value={OrderFunction}>Order</option>
            <option value={MomentFunction}>Moment</option>
            <option value={LinearRegressionFunction}>Linear Regression</option>
            <option value={FTestFunction}>F-Test</option>
            <option value={TTestFunction}>T-Test</option>
          </select>
        </div>
        {query.queryFunction.col && (
          <div className="vert-col">
            <div>
              <div>Choose Vertical</div>
              <select
                value={query.queryFunction.col.vertical}
                onChange={(e) => updateQFColVertical(e.currentTarget.value)}
              >
                {lexicon.map((li) => (
                  <option value={li.verticalId} key={'colVert' + li.verticalId}>
                    {li.verticalId}
                  </option>
                ))}
              </select>
            </div>
            <div>
              <div>Choose Column</div>
              <select
                value={query.queryFunction.col.column}
                onChange={(e) => updateQFColColumn(e.currentTarget.value)}
              >
                {getColsForVertical(query.queryFunction.col.vertical).map((cn) => (
                  <option value={cn} key={'colCol' + cn}>
                    {cn}
                  </option>
                ))}
              </select>
            </div>
          </div>
        )}
        {query.queryFunction.is_percentile !== undefined && (
          <div>
            <div>Is Percentile</div>
            <input
              type="checkbox"
              checked={query.queryFunction.is_percentile ? true : false}
              onChange={(e) => updateQFGenericValue("is_percentile", e.target.checked)}>
            </input>
          </div>
        )}
        {query.queryFunction.lowest_first !== undefined && (
          <div>
            <div>Lowest First</div>
            <input
              type="checkbox"
              checked={query.queryFunction.lowest_first ? true : false}
              onChange={(e) => updateQFGenericValue("lowest_first", e.target.checked)}></input>
          </div>
        )}
        {query.queryFunction.value !== undefined && (
          <div>
            <div>Enter Value</div>
            <input
              className="qf-val-input"
              type="number"
              value={query.queryFunction.value ? query.queryFunction.value : ''}
              onChange={(e) => updateQFGenericValue("value", e.target.value)}>
            </input>
          </div>
        )}
        {query.queryFunction.momentType && (
          <div>
            <div>Choose Moment Type</div>
            <select
              className="qf-moment-type"
              value={query.queryFunction.momentType}
              onChange={(e) => updateQFGenericValue("momentType", e.target.value)}
            >
              <option value={"COUNT"}>COUNT</option>
              <option value={"SUM"}>SUM</option>
              <option value={"MEAN"}>MEAN</option>
              <option value={"VARIANCE"}>VARIANCE</option>
              <option value={"SKEW"}>SKEW</option>
              <option value={"KURTOSIS"}>KURTOSIS</option>
            </select>
          </div>
        )}
        {query.queryFunction.dep_var && (
          <div className="vert-col">
            <div>
              <div>Choose Vertical</div>
              <select
                value={query.queryFunction.dep_var.vertical}
                onChange={(e) => updateQFDepVarVertical(e.currentTarget.value)}
              >
                {lexicon.map((li) => (
                  <option value={li.verticalId} key={'depVarVert' + li.verticalId}>
                    {li.verticalId}
                  </option>
                ))}
              </select>
            </div>
            <div>
              <div>Choose Column</div>
              <select
                value={query.queryFunction.dep_var.column}
                onChange={(e) => updateQFDepVarColumn(e.currentTarget.value)}
              >
                {getColsForVertical(query.queryFunction.dep_var.vertical).map((cn) => (
                  <option value={cn} key={'depVarCol' + cn}>
                    {cn}
                  </option>
                ))}
              </select>
            </div>
          </div>
        )}
        {query.queryFunction.indep_vars && (
          <div>
            <div>
              <div>Add Independent Variable</div>
              <FontAwesomeIcon
                className="add-icon qf-add-icon"
                icon={faPlus}
                size="lg"
                onClick={() => addIndependentVar()}
              />
            </div>
            {query.queryFunction.indep_vars.map((iv, i) => (
              <div key={"indVars" + i} className="vert-col top-spacing">
                <div>
                  <div>Choose Vertical</div>
                  <select
                    value={iv.vertical}
                    onChange={(e) => updateQFIndepVarVertical(i, e.currentTarget.value)}
                  >
                    {lexicon.map((li) => (
                      <option value={li.verticalId} key={'indepVarVert' + li.verticalId}>
                        {li.verticalId}
                      </option>
                    ))}
                  </select>
                </div>
                <div>
                  <div>Choose Column</div>
                  <select
                    value={iv.column}
                    onChange={(e) => updateQFIndepVarColumn(i, e.currentTarget.value)}
                  >
                    {getColsForVertical(iv.vertical).map((cn) => (
                      <option value={cn} key={'indepVarCol' + cn}>
                        {cn}
                      </option>
                    ))}
                  </select>
                </div>
              </div>
            ))}
          </div>
        )}
      </div>
    )
  }

  const getQueryShortDisplayMultinomial = (mn, i, j, k) => {
    return (
      <div key={"sdmn" + i + "-" + j + "-" + k} className="left-spacing-big">
        {(k > 0) && <div>AND</div>}
        <div>
          {mn.left.map((left, l) => (
            <span key={"sdmn" + i + "-" + j + "-" + k + "-" + l}>
              {(left.coefficient >= 0 && l > 0) && <span>&nbsp;+&nbsp;</span>}
              {(left.coefficient < 0) && <span>&nbsp;-&nbsp;</span>}
              {left.coefficient !== 1 && <span>{Math.abs(left.coefficient)} * </span>}
              <span>&#40;</span>
              {left.values.map((val, m) => (
                <span key={"val" + i + "-" + j + "-" + k + "-" + l + "-" + m}>
                  {(m > 0) && <span>&nbsp;*&nbsp;</span>}
                  {val.col.column}
                  {val.exp !== 1 && <span>^{ val.exp }</span>}
                  {left.values.length <= m && <span> * </span>}
                  </span>
              ))}
              <span>&#41;</span>
            </span>
          ))}
          {" " + mn.comp + " " + mn.right}
        </div>
      </div>
    )
  }

  const getQueryShortDisplayAnd = (andObj, i, j) => {
    return (
      <div key={"sdand" + i + "-" + j} className="left-spacing-big">
        {(j > 0) && <div>OR</div>}
        <div>&#40;</div>
        {andObj.map((mn, k) => getQueryShortDisplayMultinomial(mn, i, j, k))}
        <div>&#41;</div>
      </div>
    )
  }

  const getQueryShortDisplayJoin = (joinObj, index) => {
    return (
      <div key={"joshort" + index}>
        {joinObj.type + " JOIN "}{joinObj.joinOns[0].first.col.vertical + "."}
        {joinObj.joinOns[0].first.col.column + " = "}
        {joinObj.joinOns[0].second.col.vertical }.
        {joinObj.joinOns[0].second.col.column}
      </div>
    )
  }

  const getQueryShortDisplayFunction = () => {
    return (
      <div>
        {query.queryFunction.type}
        {query.queryFunction.col &&
          <span> {query.queryFunction.col.vertical} {query.queryFunction.col.column}
          </span>
        }
        {query.queryFunction.is_percentile !== undefined &&
          <div> is_percentile={query.queryFunction.is_percentile ? "true" : "false"}</div>
        }
        {query.queryFunction.lowest_first !== undefined &&
          <div> lowest_first={query.queryFunction.lowest_first ? "true" : "false"}</div>
        }
        {query.queryFunction.value !== undefined &&
          <div> value={query.queryFunction.value}</div>
        }
        {query.queryFunction.momentType &&
          <span> {query.queryFunction.momentType}</span>
        }
        {query.queryFunction.dep_var &&
          <span> {query.queryFunction.dep_var.vertical} {query.queryFunction.dep_var.column}
          </span>
        }
        {query.queryFunction.indep_vars &&
          <div>
            <div>Independent Variables:</div>
            {query.queryFunction.indep_vars.map((iv, i) => (
              <div key={"indepShort" + i}>{iv.vertical} {iv.column}</div>
            ))}
          </div>
        }
      </div>
    )
  }
  
  const getQueryShortDisplay = () => {
    return (
      <div>
        <div>WHERE</div>
        {query.preFilters.map((pf, i) =>
          (
            <div key={"sdpf" + i} className="left-spacing-big">
              <div>{verticals[i]}</div>
              <div className="left-spacing-big">
                <div>&#40;</div>
                {
                  pf.map((and, j) => getQueryShortDisplayAnd(and, i, j))
                }
                <div>&#41;</div>
              </div>
            </div>
          ))
        }
        {query.joins.length > 0 && <br />}
        {query.joins.map((join, i) => getQueryShortDisplayJoin(join, i))}
        <br />
        {getQueryShortDisplayFunction()}
      </div>
    )
  }

  const parseLexicon = () => {
    let lexFields = {};
    lexicon.forEach((v, i) => {
      lexFields[i] = [];
      v.columns.forEach((c) => {
        if (c.type === 'categorical') {
          if (lexFields[i].indexOf(c.categorical) < 0) {
            lexFields[i].push(c.categorical);
          }
        } else {
          lexFields[i].push(c.name);
        }
      });
    });

    setFields(lexFields);
  };

  const setInitialQueryFunction = () => {
    let qfunct = getInitialQueryFunctOrder()
    setQuery({
      ...query,
      queryFunction: qfunct,
    })
  }

  React.useEffect(() => {
    getLexicon(setLexicon);
  }, []);

  React.useEffect(() => {
    parseLexicon();
    setInitialQueryFunction();
  // eslint-disable-next-line
  }, [lexicon]);

  return (
    <div className="analyst-view">
      <div className="controls-panel">
        <div className="analyst-controls major-section">
          <button className="control-btn" onClick={() => clearRules()}>
            Clear
          </button>
          <h3 className="controls-title">Construct Prefilter</h3>
          {query.preFilters.length < lexicon.length && (
            <div>
              <FontAwesomeIcon
                className="add-icon"
                icon={faPlus}
                size="lg"
                onClick={() => addVertical()}
              />
              <span className="left-spacing">Preshape Vertical</span>
            </div>
          )}
          {query.preFilters.map((pf, i) => getVerticalsDisplay(i))}
        </div>
        <div className="major-section">
          <h3 className="controls-title">Construct Join Statements</h3>
          <div>
            <FontAwesomeIcon
              className="add-icon top-spacing"
              icon={faPlus}
              size="lg"
              onClick={() => addJoin()}
            />
            <span className="left-spacing">Add Join Statement</span>
          </div>
            {query.joins.map((js, i) => getJoinsDisplay(i))}
        </div>
        <h3 className="controls-title">Construct Query Function</h3>
        {getQueryFunctionDisplay()}
      </div>
      <div>
        <div className="query-row">
          <h3 className="query-title">Query</h3>
          {showQueryJson &&
            <button className="control-btn" onClick={() => setShowQueryJson(false)}>
              Show Query
            </button>
          }
          {!showQueryJson &&
            <button className="control-btn" onClick={() => setShowQueryJson(true)}>
              Show Query JSON
            </button>
          }
        </div>
        <div className="rules-box">
          {showQueryJson &&
            <pre>{JSON.stringify(query, null, 2)}</pre>
          }
          {!showQueryJson &&
            getQueryShortDisplay()
          }
        </div>
      </div>
    </div>
  );
}

export default AnalystView;
