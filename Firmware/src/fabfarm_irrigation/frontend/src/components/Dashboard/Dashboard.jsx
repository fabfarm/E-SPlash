import { useState, useEffect } from 'react';
import './Dashboard.css';

const Dashboard = ({ data }) => {
    console.log('Dashboard data:', data);

    return (
        <section className='dashboard'>
            <header className='page-title'>
                <h2>Dashboard</h2>
            </header>

            <div className='cards-container'>
                <div className='card'>
                    <div>Monday, July 10 2023</div>
                    <div className='card-value'>{data.currentTime}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data?.temperature ?? 'n/a'}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data?.humidity ?? 'n/a'}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data?.batLevel ?? 'n/a'}</div>
                </div>
            </div>
        </section>
    );
};

export default Dashboard;
