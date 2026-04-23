export default function Section({
    title,
    children,
}: {
    title: string;
    children: React.ReactNode;
}) {
    return (
        <section>
            <h2 className="text-2xl mb-6 glow">{title}</h2>
            {children}
        </section>
    );
}