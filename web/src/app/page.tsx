import Hero from "@/components/blocks/Hero";
import Projects from "@/components/blocks/Projects";
import TechStack from "@/components/blocks/TechStack";

export default function Home() {
  return (
    <main className="space-y-20 py-10">
      <Hero />
      <Projects />
      <TechStack />
      {/* TODO: Education */}
      {/* TODO: GithubStats */}
      {/* TODO: BlogPreview */}
      {/* TODO: ServerStats */}
    </main>
  );
}