import './Dashboard.css';

const Dashboard = () => {
    return (
        <section className='dashboard'>
            <header className='page-title'>
                <h2>Dashboard</h2>
            </header>

            <div className='cards-wrapper'>
                <div className='card'>
                    <div>icon</div>
                    <div>value</div>
                </div>

                <div className='card'>
                    <div>icon</div>
                    <div>value</div>
                </div>

                <div className='card'>
                    <div>icon</div>
                    <div>value</div>
                </div>

                <div className='card'>
                    <div>icon</div>
                    <div>value</div>
                </div>

                <div className='card'>
                    <div>icon</div>
                    <div>value</div>
                </div>

                <div className='card'>
                    <div>icon</div>
                    <div>value</div>
                </div>
            </div>
        </section>
    );
};

export default Dashboard;
