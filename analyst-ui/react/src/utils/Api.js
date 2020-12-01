import sampleData from './SampleData';

export const getLexicon = async (setLexicon) => {
  try {
    setLexicon(sampleData.lexicon);
  } catch (e) {
    console.log('Something went wrong');
  }
};

export const getVendorList = async (setVendorList) => {
  setVendorList([
    'Amazon',
    'Google',
    'T-Mobile',
    'AT&T',
    'Walmart',
    'Fuzzys',
    'Walgreens',
    'Hello Kitty',
    'WalYellow',
    'Goodbye Kitty',
    'Goggle',
    'Squiggle',
    'Allmart',
    'Coolios',
  ]);
};

export const getBuyerList = async (setBuyerList) => {
  setBuyerList([
    'Doug',
    'Sam',
    'Alice',
    'Bob',
    'Paul',
    'Peter',
    'Pete',
    'Pet',
    'Pe',
    'P',
    'Paul Bunn',
    'Cross Bunn',
    'Joe Cool',
    'Joe Camel',
  ]);
};
