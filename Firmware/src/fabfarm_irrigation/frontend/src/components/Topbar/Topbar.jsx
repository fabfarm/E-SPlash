import { Link } from 'react-router-dom';
import './Topbar.css';

const Topbar = () => {
    return (
        <header className='topbar'>
            <Link to='/' className='logo-wrapper'>
                <img className='logo' src='/src/assets/fabfarm_logo_cut.png' alt='logo' />
                <div className='logo-title'>FabFarm Algarve</div>
            </Link>

            {/* <div className='app-title'>Irrigation system</div> */}

            <div className='hamburger-container'>
                <Link to='/settings'>Settings</Link>
                <img className='hamburger' src='/src/assets/icons/hamburger.svg' alt='menu' />
            </div>
        </header>
    );
};

export default Topbar;
