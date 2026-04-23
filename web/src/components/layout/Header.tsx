export default function Header() {
    const links = [
        { href: "/", label: "home" },
        // { href: "/blog", label: "blog" },
    ];

    return (
        <header className="border-b border-white/10 backdrop-blur sticky top-0 z-50">
            <div className="max-w-5xl mx-auto flex justify-between py-4 px-4">
                <span className="font-bold glow">doleckijakub</span>
                <nav className="flex gap-6">
                    {links.map((l) => (
                        <a key={l.href} href={l.href} className="text-cyan-400 hover:text-cyan-300 transition">
                            {l.label}
                        </a>
                    ))}
                </nav>
            </div>
        </header>
    );
}