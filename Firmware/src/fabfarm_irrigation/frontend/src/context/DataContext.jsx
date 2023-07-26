import { createContext, useState } from 'react';

const DataContext = createContext(null);

const DataContextProvider = ({ children }) => {
    const [data, setData] = useState({ test: 'test' });

    const value = { data, setData };

    return <DataContext.Provider value={value}>{children}</DataContext.Provider>;
};

export { DataContext, DataContextProvider };
