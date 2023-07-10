import './Dashboard.css';

const Dashboard = () => {
    return (
        <section className='dashboard-container'>
            <div className='title-wrapper'>
                <h2>Dashboard</h2>
            </div>

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
