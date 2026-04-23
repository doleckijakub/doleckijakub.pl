import Section from "../ui/Section";
import { TechIcon } from "../icons/TechIcon";

const stack = [
    { id: "rust", name: "Rust" },
    { id: "php", name: "PHP" },
    { id: "python", name: "Python" },
    { id: "cplusplus", name: "C++" },
    { id: "c", name: "C" },
    { id: "typescript", name: "TypeScript" },
    { id: "javascript", name: "JavaScript" },
    { id: "html5", name: "HTML" },
    { id: "java", name: "Java" },
    { id: "docker", name: "Docker" },
    { id: "postgresql", name: "PostgreSQL" },
    { id: "mariadb", name: "MariaDB" },
    { id: "linux", name: "Linux" },
    { id: "react", name: "React" },
    { id: "nodejs", name: "Node.js" },
    { id: "nextjs", name: "Next.js" },
];

export default function TechStack() {
    return (
        <Section title="tech_stack">
            <p className="text-muted-foreground mb-6">
                Programming languages, systems and tools I use across backend, infra and web.
            </p>

            <div className="flex flex-wrap gap-3">
                {stack.map((t) => (
                    <div key={t.id} className="flex items-center gap-2 px-3 py-2 border border-white/10 rounded-md bg-white/5 hover:bg-white/10 transition">
                        <TechIcon name={t.id as any} className="w-5 h-5 object-contain" />
                        <span className="text-sm text-white/80">{t.name}</span>
                    </div>
                ))}
            </div>
        </Section>
    );
}