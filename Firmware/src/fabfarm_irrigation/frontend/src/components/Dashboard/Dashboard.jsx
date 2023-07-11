import './Dashboard.css';

const Dashboard = () => {
    return (
        <section className='dashboard'>
            <header className='page-title'>
                <h2>Dashboard</h2>
            </header>

            <div className='cards-container'>
                <div className='card'>
                    <div>Monday, July 10 2023</div>
                    <div className='card-value'>10:24</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>32 C</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>32 C</div>
                </div>

                <div className='card'>
                    <div className='icon-container'>
                        <img src='/src/assets/icons/thermometer.svg' alt='thermometer' />
                    </div>
                    <div className='card-value'>32 C</div>
                </div>
            </div>
        </section>
    );
};

export default Dashboard;
