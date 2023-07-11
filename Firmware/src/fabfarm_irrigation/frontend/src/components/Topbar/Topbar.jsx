import './Topbar.css';

const Topbar = () => {
    return (
        <header className='topbar'>
            <div className='logo-wrapper'>
                <img className='logo' src='/src/assets/fabfarm_logo_cut.png' alt='logo' />
            </div>
            <div class='topbar-title'>FABFARM Algarve</div>
            <div className='hamburger-container'>
                <img className='hamburger' src='/src/assets/icons/hamburger.svg' alt='menu' />
            </div>
        </header>
    );
};

export default Topbar;
