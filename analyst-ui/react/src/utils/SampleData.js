const sampleData = {
  sessionId: 'ffffffffffffffffffffffffffffffff',
  lexicon: [
    {
      verticalId: '1234',
      columns: [
        {
          name: 'ssn',
          type: 'integer',
          signed: false,
          bits: 30,
          operations: ['EQ'],
        },
        {
          name: 'gpa',
          type: 'real',
          precision: 2,
          scale: 7,
          operations: ['LT', 'LE', 'EQ', 'GE', 'GT'],
        },
        {
          name: 'math',
          type: 'categorical',
          categorical: 'major',
          operations: ['EQ'],
        },
        {
          name: 'history',
          type: 'categorical',
          categorical: 'major',
          operations: ['EQ'],
        },
        {
          name: 'compsci',
          type: 'categorical',
          categorical: 'major',
          operations: ['EQ'],
        },
      ],
    },
    {
      verticalId: '2345',
      columns: [
        {
          name: 'ssn',
          type: 'integer',
          signed: false,
          bits: 30,
          operations: ['EQ'],
        },
        {
          name: 'income',
          type: 'integer',
          signed: false,
          bits: 30,
          operations: ['LT', 'LE', 'EQ', 'GE', 'GT'],
        },
        {
          name: 'debt',
          type: 'integer',
          signed: false,
          bits: 30,
          operations: ['LT', 'LE', 'EQ', 'GE', 'GT'],
        },
      ],
    },
  ],
};

export default sampleData;
