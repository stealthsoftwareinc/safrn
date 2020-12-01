import React from 'react';
import { Switch, Route } from 'react-router-dom';
import './bodyContent.scss';
import AnalystView from '../analystView/AnalystView';

function BodyContent() {
  return (
    <div className="body-content">
      <Switch>
        {/* <Route path="/blockDetail">
          <InvoiceView />
        </Route> */}
        <Route path="/">
          <AnalystView />
        </Route>
      </Switch>
    </div>
  );
}

export default BodyContent;
