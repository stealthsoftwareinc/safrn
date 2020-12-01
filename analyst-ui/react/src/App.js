import React from 'react';
import './App.scss';
import { BrowserRouter as Router } from 'react-router-dom';
import { UIStore } from './store/UIStore';
import Header from './components/header/Header';
import Footer from './components/footer/Footer';
//import LeftNav from './components/leftNav/LeftNav';
import BodyContent from './components/bodyContent/BodyContent';

function App() {
  return (
    <UIStore>
      <Router>
        <div className="App">
          <Header />
          <div className="middle-section">
            {/* <LeftNav /> */}
            <BodyContent />
          </div>
          <Footer />
        </div>
      </Router>
    </UIStore>
  );
}

export default App;
