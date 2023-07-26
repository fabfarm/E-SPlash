import { useState, useEffect } from 'react';
import './Dashboard.css';

const Dashboard = ({ data }) => {
    const [currentDate, currentTime] = data.global.time.split('T');

    return (
        <section className='dashboard'>
            <header className='page-title'>
                <h2>Dashboard</h2>
            </header>

            <div className='cards-container'>
                <div className='card'>
                    <div>{currentDate}</div>
                    <div className='card-value'>{currentTime}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data.global.temperature ?? 'n/a'}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data.global.humidity ?? 'n/a'}</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>{data.global.batLevel ?? 'n/a'}</div>
                </div>
            </div>
        </section>
    );
};

export default Dashboard;
