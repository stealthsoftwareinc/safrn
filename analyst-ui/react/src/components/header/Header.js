import React from 'react';
import './header.scss';
import logoImg from '../../images/ICPSR logo.png';

function Header() {
  return (
    <div className="header">
      <img className="logo-img" src={logoImg} alt="logo" />
      <span className="title-text">SAFRN Analyst Tool</span>
    </div>
  );
}

export default Header;
