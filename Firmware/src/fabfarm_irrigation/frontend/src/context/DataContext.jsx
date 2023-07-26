import { createContext, useState, useEffect } from 'react';
// import { fetchData } from '../services';

const DataContext = createContext(null);

const DataContextProvider = ({ children }) => {
    const [data, setData] = useState(null);

    const value = { data, setData };

    // useEffect(() => {
    //     fetchData()
    //         .then((res) => res.json())
    //         .then((data) => {
    //             setData(data);
    //         });
    // }, []);

    return <DataContext.Provider value={value}>{children}</DataContext.Provider>;
};

export { DataContext, DataContextProvider };
